#include "plotting_tool.hpp"
#include "ui_plotting_tool.h"
#include "quick_plot/data_reader.hpp"
#include <sstream>

constexpr int TAB_PLOT = 0;
constexpr int TAB_DATA = 1;
constexpr int TAB_LINES = 2;
constexpr int TAB_NONEXISTENT = 3;

PlottingTool::PlottingTool() :
	ui(new Ui::PlottingTool),
	_lastTab(TAB_PLOT)
{
	ui->setupUi(this);
}

PlottingTool::~PlottingTool()
{
	delete ui;
}

void PlottingTool::on_updateDataButton_clicked()
{
	std::string dataStr = ui->data->toPlainText().toStdString();
	GraphData data;
	try {
		std::stringstream stream(dataStr);
		data = readGraph(stream, "Data");
	} catch (std::exception& e) {
		_navigator->showMessage(e.what());
	}
	_logarithmic = data.logarithmic;
	int assigning = 0;
	auto addLine = [this, &assigning] (const std::vector<double>& from, const std::string& title, bool isX) {
		auto xLine = std::make_unique<PointGraph>();
		std::vector<float> points(from.size());
		for (unsigned int i = 0; i < from.size(); i++)
			points[i] = from[i];
		xLine->changePoints(points);
		xLine->isX = isX;
		xLine->name = title;
		while (assigning < int(_graphs.size()) && !_graphs[assigning]->acceptsDataUpdate())
			assigning++;
		if (assigning < int(_graphs.size()))
			_graphs[assigning] = std::move(xLine);
		else
			_graphs.push_back(std::move(xLine));
		assigning++;
	};
	if (data.lines.size() > 0) {
		addLine(data.valuesX, data.xName.empty() ? "x" : data.xName, true);
	} else {
		addLine(data.valuesX, data.xName.empty() ? "value1" : data.xName, false);
	}
	for (auto& it : data.lines) {
		addLine(it.valuesY, it.name, false);
	}
}

void PlottingTool::load() {
	ui->plot->legend->setVisible(true);
	ui->plot->setInteraction(QCP::iRangeDrag, true);
	ui->plot->setInteraction(QCP::iRangeZoom, true);
	connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(on_resize()));
	connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), this, SLOT(on_resize()));
	int lastTabStored = _lastTab;
	_lastTab = TAB_NONEXISTENT;
	ui->tabs->setCurrentIndex(lastTabStored);
	if (_lastTab == TAB_NONEXISTENT) on_tabs_currentChanged(lastTabStored); // Loads at zero, the callback is needed
}

std::vector<std::pair<float, float>> PlottingTool::PointGraph::getPointsInRange(
		const std::vector<float>& x, float min, float max) {
	std::vector<std::pair<float, float>> result(std::min(x.size(), points.size()));
	for (unsigned int i = 0; i < x.size() && i < points.size(); i++)
		if (x[i] >= min){
			result[i].first = x[i];
			result[i].second = points[i];
			if (x[i] > max) break;
		}
	return result;
}

std::vector<float> PlottingTool::PointGraph::getX(int) {
	return points;
}

bool PlottingTool::PointGraph::changePoints(const std::vector<float>& newPoints) {
	points = newPoints;
	return true;
}

std::optional<std::pair<float, float>> PlottingTool::PointGraph::edges() {
	float min = std::numeric_limits<float>::max();
	float max = std::numeric_limits<float>::min();
	for (auto it : points) {
		if (it > max) max = it;
		if (it < min) min = it;
	}
	return std::make_pair(min, max);
}

int PlottingTool::PointGraph::size() {
	return points.size();
}

std::vector<std::string> PlottingTool::PointGraph::printColumn() const {
	std::vector<std::string> result;
	result.push_back(name);
	for (auto it : points) {
		std::stringstream out;
		out << it;
		result.push_back(out.str());
	}
	return result;
}

bool PlottingTool::PointGraph::acceptsDataUpdate() const {
	return true;
}

void PlottingTool::on_tabs_currentChanged(int index) {
	if (_lastTab == TAB_DATA) {
		on_updateDataButton_clicked();
		determineEdges();
	}

	if (index == TAB_PLOT) {
		ui->plot->clearGraphs();
		std::vector<float> x;
		int xesNeeded = 0;
		for (auto& it : _graphs) {
			xesNeeded = std::max(xesNeeded, it->size());
		}
		for (auto& it : _graphs) {
			if (it->isX) {
				x = it->getX(xesNeeded);
				ui->plot->xAxis->setLabel(QString::fromStdString(it->name));
			}
		}
		if (x.empty()) {
			for (int i = 0; i < xesNeeded; i++)
				x.push_back(i);
		}

		if(_logarithmic) {
			ui->plot->yAxis->setScaleType(QCPAxis::stLogarithmic);
			QSharedPointer<QCPAxisTickerLog> logTicker(new QCPAxisTickerLog);
			ui->plot->yAxis->setTicker(logTicker);
		}
		for (unsigned int i = 0; i < _graphs.size(); i++) {
			if (_graphs[i]->isX) continue;
			ui->plot->addGraph();
			QCPGraph* graph = ui->plot->graph(int(ui->plot->graphCount() - 1));
			graph->setPen(QColor::fromHsv(int(360.0 / _graphs.size()) * int(i), 204, 255));
			if (!_graphs[i]->continuous) {
				graph->setLineStyle(QCPGraph::lsNone);
				graph->setScatterStyle(QCPScatterStyle(QCPScatterStyle::ssPlus, 8));
			}
			graph->setName(QString::fromStdString(_graphs[i]->name));

			auto data = _graphs[i]->getPointsInRange(x, _xMin, _xMax);
			for (auto it : data) {
				if (it.second == it.second) // NaN means missing value
					graph->addData(it.first, it.second);
			}
		}

		_autoResizing = true;
		ui->plot->xAxis->setRange(_xMin, _xMax);
		ui->plot->yAxis->setRange(_yMin, _yMax);
		_autoResizing = false;
		ui->plot->replot();
	} else if (index == TAB_DATA) {
		ui->data->clear();
		if (!_graphs.empty()) {
			std::vector<std::vector<std::string>> columns;
			unsigned int first = 0;
			for (unsigned int i = 0; i < _graphs.size(); i++)
				if (_graphs[i]->isX) {
					first = i;
					break;
				}
			auto addColumn = [&columns] (Graph& graph) {
				auto got = graph.printColumn();
				if (!got.empty())
					columns.push_back(got);
			};
			addColumn(*_graphs[first]);
			for (unsigned int i = 0; i < _graphs.size(); i++)
				if (i != first)
					addColumn(*_graphs[i]);

			if (columns.size() != 0) {
				std::string text;
				for (unsigned int i = 0; i < columns[0].size(); i++) {
					if (i) text += '\n';
					for (unsigned int j = 0; j < columns.size(); j++) {
						if (j) text += '\t';
						text += columns[j][i];
					}
				}
				ui->data->setPlainText(QString::fromStdString(text));
			}
		}
	} else if (index == TAB_LINES) {

	} else throw std::runtime_error("Unknown tab opened!");
	_lastTab = index;
}

bool PlottingTool::haveX() const {
	for (auto& it : _graphs)
		if (it->isX) {
			return true;
		}
	return false;
}

void PlottingTool::determineEdges() {
	_xMin = std::numeric_limits<float>::max();
	_xMax = std::numeric_limits<float>::min();
	_yMin = std::numeric_limits<float>::max();
	_yMax = std::numeric_limits<float>::min();
	bool xExists = haveX();
	for (auto& it : _graphs) {
		auto size = it->edges();
		if (it->isX) {
			if (size) {
				_xMin = size->first;
				_xMax = size->second;
			} else xExists = false;
		} else {
			_yMin = std::min(size->first, _yMin);
			_yMax = std::max(size->second, _yMax);
		}
	}
	if (!xExists) {
		int sizeNeeded = 1;
		for (auto& it : _graphs) {
			sizeNeeded = std::max(sizeNeeded, it->size());
		}
	}
}

void PlottingTool::on_resize() {
	if (_autoResizing) return;
	auto xRange = ui->plot->xAxis->range();
	_xMin = xRange.lower;
	_xMax = xRange.upper;
	auto yRange = ui->plot->yAxis->range();
	_yMin = yRange.lower;
	_yMax = yRange.upper;
}
