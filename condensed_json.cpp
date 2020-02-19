#include "condensed_json.hpp"
#include "ui_condensed_json.h"

CondensedJSON::CondensedJSON() :
	ui(new Ui::CondensedJSON)
{
	ui->setupUi(this);
}

CondensedJSON::~CondensedJSON()
{
	delete ui;
}

void CondensedJSON::load() {
	on_toCondensedButton_clicked();
}

void CondensedJSON::on_toCondensedButton_clicked()
{
	try {
		std::stringstream stream(ui->json->toPlainText().toStdString());
		std::shared_ptr<Serialisable::JSON> parsed = Serialisable::parseJSON(stream);
		auto condensed = parsed->condensed();
		ui->condensed->setPlainText(QString::fromStdString(Serialisable::toBase64(condensed)));
	} catch (std::exception& e) {
		_navigator->showMessage(e.what());
	}
}

void CondensedJSON::on_jsonButton_clicked()
{
	try {
		std::shared_ptr<Serialisable::JSON> parsed = Serialisable::parseCondensed(
				Serialisable::fromBase64(ui->condensed->toPlainText().toStdString()));
		std::stringstream stream;
		parsed->write(stream);
		ui->json->setPlainText(QString::fromStdString(stream.str()));
	} catch (std::exception& e) {
		_navigator->showMessage(e.what());
	}
}
