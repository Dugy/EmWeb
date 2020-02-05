#ifndef DEFAULT_SCREEN_H
#define DEFAULT_SCREEN_H

#include <QWidget>

namespace Ui {
class DefaultScreen;
}

class DefaultScreen : public QWidget
{
	Q_OBJECT

public:
	explicit DefaultScreen(QWidget *parent = nullptr);
	~DefaultScreen();

private:
	Ui::DefaultScreen *ui;
};

#endif // DEFAULT_SCREEN_H
