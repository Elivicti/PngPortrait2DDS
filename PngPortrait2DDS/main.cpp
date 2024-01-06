#include "PngPortrait2DDS.h"
#include "GlobalConfigManager.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QDir>

int main(int argc, char *argv[])
{
	pybind11::scoped_interpreter guard{};

	QApplication app(argc, argv);
	QTranslator* translator = new QTranslator(&app);
	if (QLocale().language() == QLocale::Chinese)
	{
		bool success = translator->load(":/Translation/zh_cn.qm");
		if (success)
			app.installTranslator(translator);
	}

#ifndef _DEBUG	// do not set to application path if in debug mode
	QDir::setCurrent(app.applicationDirPath());
#endif

	GlobalConfigManager::init();

	PngPortrait2DDS w;
	w.show();
	int code = app.exec();

	GlobalConfigManager::release();
	return code;
}
