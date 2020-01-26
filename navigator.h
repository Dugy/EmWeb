#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <QMainWindow>

namespace Ui {
	class Navigator;
}

class Navigator : public QMainWindow
{
	Q_OBJECT

public:
	explicit Navigator(QWidget *parent = 0);
	~Navigator();

private:
	Ui::Navigator *ui;
};

#endif // NAVIGATOR_H
