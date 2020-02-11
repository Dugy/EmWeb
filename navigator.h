#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QMainWindow>
#include "i_navigator.h"
#include "content_base.hpp"

namespace Ui {
	class Navigator;
}

class ComponentsList;

class Navigator : public QMainWindow, public iNavigator, public Serialisable
{
	Q_OBJECT

public:
	explicit Navigator(QWidget *parent = 0);
	~Navigator();

	void updateSettings() override;
	void setWidget(const std::string& name) override;

	void language(Language lang) override;
	Language language() const override;
	void showMessage(const std::string& message) override;
	void goFullscreen() override;

private:
	Ui::Navigator *ui;
	ComponentsList* _componentsList = nullptr;

	bool _fullscreen = false;
	Language _language = Language::ENGLISH;
	std::unique_ptr<ContentBase> _contents;
	QWidget* _central = nullptr;

	void serialisation() override {
		synch("full", _fullscreen);
		synch("lang", _language);
		synch("cont", _contents);
	}

	void setupContents();
};

#endif // NAVIGATOR_H
