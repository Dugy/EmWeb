#include "components_list.h"
#include "ui_components_list.h"
#include "content_base.hpp"

ComponentsList::ComponentsList(iNavigator* navigator) :
	QWidget(),
	ui(new Ui::ComponentsList),
	_navigator(navigator)
{
	ui->setupUi(this);
}

ComponentsList::~ComponentsList()
{
	delete ui;
}

void ComponentsList::on_fullscreen_clicked()
{
	_navigator->goFullscreen();
}

void ComponentsList::on_bullshit_clicked()
{
	_navigator->setWidget("bull");
}

void ComponentsList::on_pushButton_clicked()
{
	_navigator->setWidget("cfg");
}

void ComponentsList::on_pushButton_2_clicked()
{
	_navigator->setWidget("evil");
}
