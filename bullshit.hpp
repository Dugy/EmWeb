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
	}

private:
    Ui::Bullshit *ui;
    SERIALISABLE_REGISTER_POLYMORPHIC(ContentBase, Bullshit, "bull");
};

#endif // BULLSHIT_HPP
