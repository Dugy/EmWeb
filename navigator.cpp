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

	{
		QColor darkGrey = QColor(60, 60, 60);
		QColor midDarkGrey = QColor(40, 40, 40);
		QColor veryDarkGrey = QColor(15, 15, 15);
		QColor link = QColor(42, 130, 218);
		QColor linkVisited = QColor(165, 112, 255);
		QColor hightlight = QColor(0, 100, 170);
		QColor midlight = QColor(240, 240, 240);
		QColor mid = QColor(160, 160, 160);

		QPalette pal(this->palette());
		auto fill = [&] (QPalette::ColorRole role, QColor normal, QColor inactive, QColor disabled) {
			pal.setColor(role, normal);
			pal.setColor(QPalette::Inactive, role, inactive);
			pal.setColor(QPalette::Disabled, role, disabled);
		};
		fill(QPalette::WindowText, Qt::white, Qt::white, QColor(200, 200, 200));
		fill(QPalette::Button, darkGrey, darkGrey, midDarkGrey);
		fill(QPalette::Light, QColor("#c0c0c0"), QColor("#c0c0c0"), Qt::black);
		fill(QPalette::Midlight, midlight, midlight, midlight);
		fill(QPalette::Mid, mid, mid, mid);
		fill(QPalette::Dark, darkGrey, darkGrey, darkGrey);
		fill(QPalette::Text, Qt::white, Qt::white, Qt::lightGray);
		fill(QPalette::BrightText, Qt::red, Qt::red, Qt::darkRed);
		fill(QPalette::ButtonText, Qt::white, Qt::white, Qt::lightGray);
		fill(QPalette::Base, veryDarkGrey, veryDarkGrey, midDarkGrey);
		fill(QPalette::Window, darkGrey, darkGrey, midDarkGrey);
		fill(QPalette::Shadow, midDarkGrey, QColor("#696969"), Qt::black);
		fill(QPalette::Highlight, hightlight, hightlight, hightlight);
		fill(QPalette::HighlightedText, Qt::black, Qt::black, Qt::darkGray);
		fill(QPalette::Link, link, link, link);
		fill(QPalette::LinkVisited, linkVisited, linkVisited, linkVisited);
		fill(QPalette::AlternateBase, darkGrey, darkGrey, midDarkGrey);
		fill(QPalette::NoRole, Qt::black, Qt::black, Qt::black);
		fill(QPalette::ToolTipBase, Qt::lightGray, Qt::lightGray, Qt::lightGray);
		fill(QPalette::ToolTipText, Qt::black, Qt::black, Qt::darkGray);
		setPalette(pal);
	}

	try {
		auto settings = sessionSettings();
		fromJSON(settings);
		if (_contents) {
			ui->layout->insertWidget(MAIN, _contents.get(), 1);
			setupContents();
		}
	} catch (std::exception& e) {
		std::cerr << "Bad data, " << e.what() << ", going default" << std::endl;
	}

	if (!_contents) {
		_central = new DefaultScreen;
		ui->layout->insertWidget(MAIN, _central, 1);
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
	try {
		_contents.release(); // The pointer is kept in _central
		_contents = GenericFactory<ContentBase>::createChild(name);
		ui->layout->replaceWidget(_central, _contents.get());
		delete _central;
		setupContents();
		updateSettings();
	} catch(std::exception& e) {
		showMessage(e.what());
	}
}

void Navigator::updateSettings() {
	sessionSettings(toJSON());
}

void Navigator::language(Language lang) {
	_language = lang;
	updateSettings();
}

Language Navigator::language() const {
	return _language;
}

void Navigator::showMessage(const std::string& message) {
	ui->statusBar->showMessage(QString::fromStdString(message));
}

void Navigator::goFullscreen() {
	_fullscreen = true;
	ui->layout->removeWidget(_componentsList);
	delete _componentsList;
	_componentsList = nullptr;
	updateSettings();
}

void Navigator::setupContents() {
	_central = _contents.get();
	_contents->setNavigator(this);
	_contents->load();
}
