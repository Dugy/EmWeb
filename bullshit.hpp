#ifndef BULLSHIT_HPP
#define BULLSHIT_HPP

#include "content_base.hpp"

namespace Ui {
class Bullshit;
}

class Bullshit : public ContentBase
{
    Q_OBJECT

public:
    explicit Bullshit();
    ~Bullshit();

	void serialisation() override {
		ContentBase::serialisation();
		subclass("bull");
		synch("value", _contentIndentifier);
	}

private:
    Ui::Bullshit *ui;
	std::string _contentIndentifier;
    SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, Bullshit, "bull");
	std::vector<QWidget*> _elements;

	void load() override;
};

#endif // BULLSHIT_HPP
