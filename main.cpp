#include "quickplot.h"
#include <iostream>
#include <fstream>
#include <QApplication>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "Use " << argv[0] << " [file_name]" << std::endl;
		return 1;
	}
	QApplication app(argc, argv);
	QuickPlot window;
	int filesRead = 0;
	for (int i = 1; i < argc; i++) {
		try {
			window.addFile(argv[i]);
			filesRead++;
		} catch (std::exception& e) {
			std::cerr << "Could not read " << argv[i] << ": " << e.what() << std::endl;
		} catch (...) {
			std::cerr << "Could not read " << argv[i] << ": No idea why" << std::endl;
		}
	}
	if (!filesRead)
		return 2;
	window.show();
	return app.exec();
}
