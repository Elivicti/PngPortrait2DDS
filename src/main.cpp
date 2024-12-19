#include "PngPortrait2DDS.h"
#include "GlobalConfigManager.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QDir>

#include "ErrorDialog.h"

int main(int argc, char *argv[])
{
	QApplication app{ argc,argv };
	QTranslator* translator = new QTranslator(&app);
	if (QLocale{}.language() == QLocale::Chinese)
	{
		bool success = translator->load(":/Translation/zh_cn.qm");
		if (success)
			app.installTranslator(translator);
	}

#ifndef _DEBUG
	// do not set to application path if in debug mode
	QDir::setCurrent(app.applicationDirPath());

	auto old_stdout = *stderr;
	auto p = std::freopen("launch.log", "w+", stderr);
#endif

	try
	{
		pybind11::scoped_interpreter guard{};
		PngPortrait2DDS w;
		w.show();

		return app.exec();
	}
#ifndef _DEBUG
	catch (const std::exception& e)
	{
		*stderr = old_stdout;
		QFile temp{ "launch.log" };
		if (!temp.open(QIODevice::ReadOnly))
		{
			qDebug() << "somehow failed: " << temp.errorString();
		}

		QString msg{ temp.readAll() };
		
		if (!temp.remove())
		{
			qDebug() << "somehow failed: " << temp.errorString();
		}

		ErrorDialog msg_box{ QObject::tr("Critical Error"), msg += e.what() };
		msg_box.setHelperText(QObject::tr(
			"If \"Python\" is mentioned in the error message, "
			"then this problem is most likely caused by a missing or corrupted Python install. "
			"Try installing/reinstalling Python."
		));

		return msg_box.exec();
	}
#else
	catch (const std::exception& e)
	{
		qDebug() << typeid(e).name();
		qDebug() << e.what();
	}
#endif

	return app.exec();
}
