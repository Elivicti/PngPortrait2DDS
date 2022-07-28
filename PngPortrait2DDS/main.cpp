#include "PngPortrait2DDS.h"
#include <QtWidgets/QApplication>
#include <QTranslator>


int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	QTranslator* translator = new QTranslator(&app);

	if (QLocale().language() == QLocale::Chinese)
	{
		bool success = translator->load(":/Translation/zh_cn.qm");
		if (success)
			app.installTranslator(translator);
	}

	PngPortrait2DDS w;
	w.show();
	return app.exec();
}
