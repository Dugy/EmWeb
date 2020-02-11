#ifndef TEXT_GENERATOR_HPP
#define TEXT_GENERATOR_HPP

#include "content_base.hpp"

namespace Ui {
class TextGenerator;
}

class TextGenerator : public ContentBase
{
	Q_OBJECT

public:
	explicit TextGenerator();
	~TextGenerator();

	void serialisation() override {
		ContentBase::serialisation();
		subclass("cfg");
		synch("source", _source);
	}

	void load() override;

private slots:
	void on_generate_clicked();

private:
	Ui::TextGenerator *ui;
	std::string _source = "main={Hello}, {visitor}!\nHello=Hello|Hey|Welcome\nvisitor=visitor|friend|pal";
	SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, TextGenerator, "cfg");
};

#endif // TEXT_GENERATOR_HPP
