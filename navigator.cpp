#include "navigator.h"
#include "ui_navigator.h"

Navigator::Navigator(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Navigator)
{
	ui->setupUi(this);
}

Navigator::~Navigator()
{
	delete ui;
}
