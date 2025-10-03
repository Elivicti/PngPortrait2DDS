#include <QApplication>

#include "windows/MainWindow.h"
#include "Settings.h"

static QtFileSystem::Path get_program_dir(const char* argv0)
{
	std::filesystem::path p{ argv0 };
	if (std::filesystem::is_symlink(p))
		p = std::filesystem::read_symlink(p);

	return p.parent_path();
}

#if !defined(NDEBUG) && !defined(_DEBUG)
#define _DEBUG 1
#endif

int main(int argc, char* argv[])
{
	QApplication app{ argc, argv };

#ifdef _DEBUG
	SettingsManager::init("settings.ini");
#else
	QtFileSystem::Path program_dir{ get_program_dir(argv[0]) };
	SettingsManager::init(program_dir / "settings.ini");
#endif

	MainWindow w;
	w.show();

	return app.exec();
}
