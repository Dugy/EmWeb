#include "navigator.h"
#include <QApplication>

int main(int argc, char *argv[])
{
	srand(time(nullptr));

	QApplication a(argc, argv);
	Navigator w;
	w.show();

	return a.exec();
}
