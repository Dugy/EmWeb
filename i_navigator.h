#ifndef INAVIGATOR_H
#define INAVIGATOR_H

#include <string>

enum class Language {
	ENGLISH,
	SLOVAK
};

struct iNavigator {
	virtual void updateSettings() = 0;
	virtual void setWidget(const std::string& name) = 0;
	virtual void showMessage(const std::string& message) = 0;
	virtual Language language() const = 0;
	virtual void language(Language lang) = 0;
	virtual void goFullscreen() = 0;
};

#endif // INAVIGATOR_H
