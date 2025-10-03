#pragma once

#include "utils/QtFileSystem.h"

#include <QSettings>
#include <QPoint>
#include <QSize>

struct Settings
{
	double default_scale  = 0.5;
	QPoint default_offset = { 0, 0 };
	QSize  default_size   = { 496, 380 };


	int control_max_width  = 8096;
	int control_max_height = 8096;

	double control_max_scale          = 2.0;
	double control_wheel_step         = 0.05;
	double control_scale_spinbox_step = 0.05;
};


class SettingsManager
{
public:
	SettingsManager(const SettingsManager&) = delete;
	SettingsManager(SettingsManager&&) = delete;
	~SettingsManager() = default;

	static SettingsManager& init(const QtFileSystem::Path& ini_path);
	static SettingsManager& instance();

	const Settings& settings() { return val; }
	static Settings default_settings() { return Settings{}; }

	void save(const QtFileSystem::Path& ini_path);
	void save();

protected:
	SettingsManager(const QtFileSystem::Path& ini_path);
private:
	Settings val;
	QtFileSystem::Path path;


	static std::unique_ptr<SettingsManager> inst;
};
