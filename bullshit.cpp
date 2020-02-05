#include "bullshit.hpp"
#include "ui_bullshit.h"

Bullshit::Bullshit() :
	ui(new Ui::Bullshit)
{
	ui->setupUi(this);
	QString contents = "_blablabla_";
	ui->_contents->setMarkdown(contents);

	ui->_title->setText("Blabla");
}

Bullshit::~Bullshit()
{
	delete ui;
}
