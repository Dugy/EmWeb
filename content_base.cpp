#include "content_base.hpp"

ContentBase::~ContentBase() {
}

void ContentBase::setNavigator(iNavigator* navigator) {
	_navigator = navigator;
}
