#ifndef INAVIGATOR_H
#define INAVIGATOR_H

#include <string>

struct iNavigator {
	virtual void updateSettings() = 0;
	virtual void setWidget(const std::string& name) = 0;
};

#endif // INAVIGATOR_H
