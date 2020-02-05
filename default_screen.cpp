#include "default_screen.h"
#include "ui_default_screen.h"

DefaultScreen::DefaultScreen(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::DefaultScreen)
{
	ui->setupUi(this);
}

DefaultScreen::~DefaultScreen()
{
	delete ui;
}
