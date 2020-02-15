#include <fstream>
#include <sstream>
#include <iostream>
#include <QFont>
#include "quickplot.h"
#include "ui_quickplot.h"
#include "qcustomplot.h"

constexpr double POINTS_RELATIVE_THRESHOLD = 0.02;
constexpr int POINTS_MIN_THRESHOLD = 10;
constexpr int POINTS_MAX_THRESHOLD = 1000;

QuickPlot::QuickPlot(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::QuickPlot)
{
	ui->setupUi(this);
	connect(ui->actionExit, &QAction::triggered, this, &QuickPlot::on_exitButton_clicked);
}

QuickPlot::~QuickPlot()
{
	delete ui;
}

void QuickPlot::showEvent(QShowEvent* event) {
	int windowSize = 1;
	while (windowSize * windowSize < int(_graphs.size()))
		windowSize++;

	for (int i = 0; i < int(_graphs.size()); i++) {
		GraphData& data = _graphs[size_t(i)];
		QCustomPlot* plot = new QCustomPlot(this);
		plot->plotLayout()->insertRow(0);
		plot->plotLayout()->addElement(0, 0, new QCPTextElement(plot, QString::fromStdString(data.name), QFont("sans", 12, QFont::Bold)));
		ui->layout->addWidget(plot, i / windowSize, i % windowSize);
		plot->setInteraction(QCP::iRangeDrag, true);
		plot->setInteraction(QCP::iRangeZoom, true);
		if(data.logarithmic) {
			plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
			plot->yAxis->setTicker(logTicker);
		}

		for (unsigned int i = 0; i < data.lines.size(); i++) {
			plot->addGraph();
			QCPGraph* graph = plot->graph(int(i));
			graph->setPen(QColor::fromHsv(int(360.0 / data.lines.size()) * int(i), 204, 255));
			int validPoints = data.lines[i].validPoints;
			if (validPoints < POINTS_MIN_THRESHOLD || (validPoints < POINTS_MAX_THRESHOLD && validPoints < data.maxValidPoints * POINTS_RELATIVE_THRESHOLD)) {
				graph->setLineStyle(QCPGraph::lsNone);
				graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 8));
			}
		}

		if (data.named) {
			plot->xAxis->setLabel(QString::fromStdString(data.xName));
			for (int i = 0; i < int(data.lines.size()); i++) {
				plot->graph(i)->setName(QString::fromStdString(data.lines[size_t(i)].name));
			}
			plot->legend->setVisible(true);
		}

		for (unsigned int i = 0; i < data.lines.size(); i++) {
			QCPGraph* graph = plot->graph(int(i));
			for (unsigned int j = 0; j < data.valuesX.size(); j++) {
				double value = data.lines[i].valuesY[j];
				if (value == value) // NaN means missing value
					graph->addData(data.valuesX[j], value);
			}
		}
		plot->rescaleAxes();
		plot->replot();
	}
	QWidget::showEvent(event);
}

void QuickPlot::addFile(const std::string& fileName) {
	std::ifstream file(fileName);
	if (!file.good())
		throw(std::runtime_error("Could not open file: " + fileName));

	_graphs.push_back(readGraph(file, fileName));
}

void QuickPlot::on_exitButton_clicked()
{
	QApplication::quit();
}
