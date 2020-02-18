#ifndef COMPONENTS_LIST_H
#define COMPONENTS_LIST_H

#include <QWidget>
#include <i_navigator.h>

namespace Ui {
class ComponentsList;
}

class ComponentsList : public QWidget
{
	Q_OBJECT

public:
	explicit ComponentsList(iNavigator* navigator);
	~ComponentsList();

private slots:

	void on_fullscreen_clicked();

	void on_bullshit_clicked();

	void on_contextfreeGenerator_clicked();

	void on_blackspeechTranslator_clicked();

	void on_plottingTool_clicked();

private:
	Ui::ComponentsList *ui;
	iNavigator* _navigator;
};

#endif // COMPONENTS_LIST_H
