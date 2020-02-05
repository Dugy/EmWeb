#include <iostream>
#include "navigator.h"
#include "ui_navigator.h"
#include "environment.h"
#include "default_screen.h"
#include "components_list.h"
#include <QLabel>

constexpr int LEFT_BAR = 0;
constexpr int MAIN = 1;
//constexpr int RIGHT_BAR = 2;

Navigator::Navigator(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Navigator)
{
	ui->setupUi(this);
	try {
		auto settings = sessionSettings();
		fromJSON(settings);
		if (_contents) {
			_contents->setNavigator(this);
			ui->layout->insertWidget(MAIN, _contents.get(), 1);
		}
	} catch (std::exception& e) {
		std::cerr << "Bad data, going default" << std::endl;
	}

	if (!_contents) {
		ui->layout->insertWidget(MAIN, new DefaultScreen, 1);
	}

	if (!_fullscreen) {
		_componentsList = new ComponentsList(this);
		ui->layout->insertWidget(LEFT_BAR, _componentsList, 0);
	}
}

Navigator::~Navigator()
{
	sessionSettings(toJSON());
	delete ui;
}

void Navigator::setWidget(const std::string& name) {
	auto removed = ui->layout->itemAt(MAIN);
	_contents = GenericFactory<ContentBase>::createChild(name);
	ui->layout->replaceWidget(removed->widget(), _contents.get());
	delete removed->widget();
}

void Navigator::updateSettings() {
	sessionSettings(toJSON());
}
