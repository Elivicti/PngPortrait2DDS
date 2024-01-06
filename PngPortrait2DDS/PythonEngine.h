#pragma once
#include "PresetManager.h"
#undef slots
#include <pybind11/embed.h>
#include <pybind11/stl.h>
#define slots Q_SLOTS
#include <fstream>
#include <QDir>

class QMenu;
class QActionGroup;

class PythonLogger
{
public:
	PythonLogger(const std::string& logfile);
	~PythonLogger() = default;

	void debug(const std::string& msg);
	void info(const std::string& msg);
	void warning(const std::string& msg);
	void critical(const std::string& msg);
	void fatal(const std::string& msg);

	using shared_ptr = std::shared_ptr<PythonLogger>;

	// must be called within py::scoped_interpreter's lifetime
	void reopen(const std::string& log);

	void init(const QString& script);

public:
	void cpp_info_log(const std::string& msg);

private:
	std::fstream log_stream;
	pybind11::object calling_frame;	// the frame when function is called
	std::string script;
};


class PythonEngine : public QObject
{
	Q_OBJECT
public:
	PythonEngine(QWidget* parent = nullptr);

	bool runScript(const PresetData& data, const QString& name, bool registration, bool proper_name_effect,
		std::function<void(const std::vector<std::string>&)> registration_callback,
		const QString& output_path = QString());
	const QString& errorMessage() const { return error_str; }

	void setUpScriptMenu(QMenu* menu);

	// @param script should be full name(with suffix i.e "default.py")
	void setEnabledScript(const QString& script);

	QString enabledScript() const;

private:
	void defaultScriptsCheck();

private:
	PythonLogger::shared_ptr logger;
	QString error_str;

	QDir script_dir;

	QWidget* mainwindow;
	QMenu* script_menu;
	QActionGroup* script_group;
};

