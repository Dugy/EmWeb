#ifndef CONDENSED_JSON_HPP
#define CONDENSED_JSON_HPP

#include "content_base.hpp"

namespace Ui {
class CondensedJSON;
}

class CondensedJSON : public ContentBase
{
	Q_OBJECT

public:
	explicit CondensedJSON();
	~CondensedJSON();

	void serialisation() override {
		ContentBase::serialisation();
		subclass("cjson");
	}

	void load() override;

private slots:
	void on_toCondensedButton_clicked();

	void on_jsonButton_clicked();

	void on_json_textChanged();

	void on_condensed_textChanged();

	void on_uploadBinary_clicked();

	void on_downloadBinary_clicked();

private:
	Ui::CondensedJSON *ui;
	SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, CondensedJSON, "cjson");

	void normaliseButtons();
};

#endif // CONDENSED_JSON_HPP
