#ifndef QUICKPLOT_H
#define QUICKPLOT_H

#include <QMainWindow>
#include <string>
#include <vector>
#include "data_reader.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class QuickPlot; }
QT_END_NAMESPACE

class QuickPlot : public QMainWindow
{
	Q_OBJECT

public:
	QuickPlot(QWidget *parent = nullptr);
	~QuickPlot();
	void addFile(const std::string& fileName);
	void showEvent(QShowEvent* event);

private slots:
	void on_exitButton_clicked();

private:
	Ui::QuickPlot *ui;

	std::vector<GraphData> _graphs;
};
#endif // QUICKPLOT_H
