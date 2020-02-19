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

private:
	Ui::CondensedJSON *ui;
	SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, CondensedJSON, "cjson");
};

#endif // CONDENSED_JSON_HPP
