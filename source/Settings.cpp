#include "Settings.h"
#include "utils/misc.h"

#include <SimpleReflect/Reflect.hpp>

REFLECT_DEFINE_GLOBAL(Settings) {
	REFLECT_DEFINE() {
		REFLECT_MEMBER(TXT("default/scale"),  default_scale),
		REFLECT_MEMBER(TXT("default/offset"), default_offset),
		REFLECT_MEMBER(TXT("default/size"),   default_size),

		REFLECT_MEMBER(TXT("control/max_height"), control_max_height),
		REFLECT_MEMBER(TXT("control/max_width"),  control_max_width),
		REFLECT_MEMBER(TXT("control/max_scale"),  control_max_scale),
		REFLECT_MEMBER(TXT("control/wheel_step"), control_wheel_step),
		REFLECT_MEMBER(TXT("control/scale_spinbox_step"), control_scale_spinbox_step),
	};
};

struct Settings_serializer
{
	QSettings& ini;

	template<typename T>
	void operator()(const Settings*, Reflect::StringView name, const T& val)
	{ ini.setValue(name, val); }
};

struct Settings_deserializer
{
	const QSettings& ini;

	template<typename T>
	void operator()(const Settings*, Reflect::StringView name, T& val) const
	{
		if(!ini.contains(name))
			return;
		auto var = ini.value(name);
		if (!var.canConvert<T>())
			return;
		val = ini.value(name).value<T>();
	}
};


std::unique_ptr<SettingsManager> SettingsManager::inst = {};

SettingsManager& SettingsManager::init(const QtFileSystem::Path& ini_path)
{ inst.reset(new SettingsManager{ ini_path }); return *inst; }
SettingsManager& SettingsManager::instance()
{ return *inst; }


SettingsManager::SettingsManager(const QtFileSystem::Path& ini_path)
	: val{}, path{ ini_path }
{
	QSettings ini{ ini_path, QSettings::IniFormat };
	Settings_deserializer s{ ini };
	Reflect::for_each_member(&val, s);
}


void SettingsManager::save(const QtFileSystem::Path& ini_path)
{
	QSettings ini{ ini_path, QSettings::IniFormat };

	Settings_serializer s{ ini };
	Reflect::for_each_member(&val, s);
	ini.sync();

}
void SettingsManager::save()
{ save(path); }
