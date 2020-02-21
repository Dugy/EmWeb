#include "condensed_json.hpp"
#include "ui_condensed_json.h"
#include "environment.h"
#include <QMessageBox>

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
		normaliseButtons();
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
		normaliseButtons();
	} catch (std::exception& e) {
		_navigator->showMessage(e.what());
	}
}

void CondensedJSON::on_json_textChanged()
{
	ui->jsonButton->setText("To JSON (overwrite)");
}

void CondensedJSON::on_condensed_textChanged()
{
	ui->toCondensedButton->setText("To Condensed (overwrite)");
}

void CondensedJSON::normaliseButtons() {
	ui->jsonButton->setText("To JSON");
	ui->toCondensedButton->setText("To Condensed");
}

void CondensedJSON::on_uploadBinary_clicked()
{
	uploadFile([this] (const std::vector<uint8_t>& uploaded) {
		ui->condensed->setPlainText(QString::fromStdString(Serialisable::toBase64(uploaded)));
	});
}

void CondensedJSON::on_downloadBinary_clicked()
{
	auto downloaded = Serialisable::fromBase64(ui->condensed->toPlainText().toStdString());
	downloadFile(downloaded);
}
