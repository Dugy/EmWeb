#include "text_generator.hpp"
#include "ui_text_generator.h"
#include "contextfree_generator/cfg_generator.hpp"

TextGenerator::TextGenerator() :
	ui(new Ui::TextGenerator)
{
	ui->setupUi(this);
}

TextGenerator::~TextGenerator()
{
	delete ui;
}

void TextGenerator::load() {
	ui->input->setPlainText(QString::fromStdString(_source));
	on_generate_clicked();
}

void TextGenerator::on_generate_clicked()
{
	try {
		_source = ui->input->toPlainText().toStdString();
		CfgGenerator gen;
		gen.constructFromString(_source);
		ui->output->setMarkdown(QString::fromStdString(gen.generate(rand())));
		ui->problems->setText("");
	} catch(std::exception& e) {
		ui->problems->setText(e.what());
	}
	_navigator->updateSettings();
}
