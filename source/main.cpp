#include <QApplication>

#include "windows/MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication app{ argc, argv };
	MainWindow w;
	w.show();

	return app.exec();
}
