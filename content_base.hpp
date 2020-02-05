#ifndef CONTENT_BASE_HPP
#define CONTENT_BASE_HPP
#include "serialisable/serialisable_polymorphic.hpp"
#include "i_navigator.h"
#include <QWidget>

enum class Language {
	ENGLISH,
	SLOVAK
};

struct ContentBase : public QWidget, public SerialisablePolymorphic {
	Q_OBJECT
public:
	virtual ~ContentBase();

	void setNavigator(iNavigator* navigator);

protected:
	iNavigator* _navigator = nullptr;

	void serialisation() override {
	}
};

#endif // CONTENT_BASE_HPP
