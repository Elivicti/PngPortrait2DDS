#include "PngPortrait2DDS.h"
#include "GlobalConfigManager.h"
#include <QtWidgets/QApplication>
#include <QTranslator>
#include <QDir>

#include "ErrorDialog.h"

struct ScopedRedirect
{
	ScopedRedirect(const char* fname)
		: og_stdout{ *stdout }
		, launch_log{ std::freopen(fname, "w+", stderr) }
		, log{ fname }
	{}

	~ScopedRedirect()
	{
		close_log();
		remove();
	}

	void close_log()
	{
		std::fclose(launch_log);
		if (!log.isOpen())
			log.open(QIODevice::ReadOnly);
		*stdout = og_stdout;
	}

	QString read_all()
	{
		if (!log.isOpen())
		{
			return log.errorString();
		}
		return log.readAll();
	}

	void remove()
	{
		if (log.remove())
			return;
		close_log();
		log.remove();
	}

	QString read_and_close()
	{
		close_log();
		QString content = read_all();
		remove();
		return content;
	}

	std::FILE og_stdout;
	std::FILE* launch_log;
	QFile log;
};

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

	ScopedRedirect r{ "launch.log" };
#endif

	try
	{
		pybind11::scoped_interpreter guard{};
		PngPortrait2DDS w;
		w.show();
#ifndef _DEBUG
		r.remove();
#endif
		return app.exec();
	}
#ifndef _DEBUG
	catch (const std::exception& e)
	{
		QString msg{ r.read_and_close() };

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
