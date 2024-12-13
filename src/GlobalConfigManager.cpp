#include "GlobalConfigManager.h"
#include <QDir>

#include <QDebug>

GlobalConfigManager* GlobalConfigManager::manager = nullptr;

#define GROUP_Image(k)    "Image/" k
#define GROUP_Portrait(k) "Portrait/" k
#define GROUP_Export(k)   "Export/" k


const int    HardCodedDefault::H_KEY_Width   = 280;
const int    HardCodedDefault::H_KEY_Height  = 160;
const double HardCodedDefault::H_KEY_Scale   = 0.20;
const int    HardCodedDefault::H_KEY_OffsetX = 0;
const int    HardCodedDefault::H_KEY_OffsetY = 0;
const bool   HardCodedDefault::H_KEY_UseForSpecies = true;
const bool   HardCodedDefault::H_KEY_UseForLeaders = true;
const bool   HardCodedDefault::H_KEY_UseForRulers  = true;
const char * HardCodedDefault::H_KEY_Script = "default.py";
const bool   HardCodedDefault::H_KEY_DDS              = true;
const bool   HardCodedDefault::H_KEY_Registration     = false;
const bool   HardCodedDefault::H_KEY_ExtraEffect = false;


GlobalConfigManager::GlobalConfigManager()
	: ini{ "config.ini", QSettings::IniFormat }
{
	commentCheck();
	defaultValueCheck();
}

void GlobalConfigManager::defaultValueCheck()
{
	if (!ini.contains(GROUP_Image(KEY_Width)) ||
		(ini.value(GROUP_Image(KEY_Width)).type() != QVariant::Int))
		ini.setValue(GROUP_Image(KEY_Width), 280);

	if (!ini.contains(GROUP_Image(KEY_Height)) ||
		(ini.value(GROUP_Image(KEY_Height)).type() != QVariant::Int))
		ini.setValue(GROUP_Image(KEY_Height), 160);

	if (!ini.contains(GROUP_Portrait(KEY_Scale)) ||
		(ini.value(GROUP_Portrait(KEY_Scale)).type() != QVariant::Double))
		ini.setValue(GROUP_Portrait(KEY_Scale), 0.20);

	if (!ini.contains(GROUP_Portrait(KEY_OffsetX)) ||
		(ini.value(GROUP_Portrait(KEY_OffsetX)).type() != QVariant::Int))
		ini.setValue(GROUP_Portrait(KEY_OffsetX), 0);

	if (!ini.contains(GROUP_Portrait(KEY_OffsetY)) ||
		(ini.value(GROUP_Portrait(KEY_OffsetY)).type() != QVariant::Int))
		ini.setValue(GROUP_Portrait(KEY_OffsetY), 0);

	if (!ini.contains(GROUP_Portrait(KEY_UseForSpecies)) ||
		(ini.value(GROUP_Portrait(KEY_UseForSpecies)).type() != QVariant::Bool))
		ini.setValue(GROUP_Portrait(KEY_UseForSpecies), true);

	if (!ini.contains(GROUP_Portrait(KEY_UseForLeaders)) ||
		(ini.value(GROUP_Portrait(KEY_UseForLeaders)).type() != QVariant::Bool))
		ini.setValue(GROUP_Portrait(KEY_UseForLeaders), true);

	if (!ini.contains(GROUP_Portrait(KEY_UseForRulers)) ||
		(ini.value(GROUP_Portrait(KEY_UseForRulers)).type() != QVariant::Bool))
		ini.setValue(GROUP_Portrait(KEY_UseForRulers), true);

	if (!ini.contains(GROUP_Export(KEY_Script)) ||
		(ini.value(GROUP_Export(KEY_Script)).type() != QVariant::String))
		ini.setValue(GROUP_Export(KEY_Script), "default.py");

	if (!ini.contains(GROUP_Export(KEY_DDS)) ||
		(ini.value(GROUP_Export(KEY_DDS)).type() != QVariant::Bool))
		ini.setValue(GROUP_Export(KEY_DDS), true);

	if (!ini.contains(GROUP_Export(KEY_Registration)) ||
		(ini.value(GROUP_Export(KEY_Registration)).type() != QVariant::Bool))
		ini.setValue(GROUP_Export(KEY_Registration), false);

	if (!ini.contains(GROUP_Export(KEY_ExtraEffect)) ||
		(ini.value(GROUP_Export(KEY_ExtraEffect)).type() != QVariant::Bool))
		ini.setValue(GROUP_Export(KEY_ExtraEffect), false);
}
void GlobalConfigManager::commentCheck()
{
	ini.setValue("other/comment00", "this is config of Portrait2DDS program, which defines the default value of some properties");
	ini.setValue("other/comment01", "value types:");
	ini.setValue("other/comment02", "Width:   int");
	ini.setValue("other/comment03", "Height:  int");
	ini.setValue("other/comment04", "Scale:   double");
	ini.setValue("other/comment05", "OffsetX: int");
	ini.setValue("other/comment06", "OffsetY: int");
	ini.setValue("other/comment07", "UseForSpecies: bool");
	ini.setValue("other/comment08", "UseForLeaders: bool");
	ini.setValue("other/comment09", "UseForRulers:  bool");
	ini.setValue("other/comment10", "Script:     string");
	ini.setValue("other/comment11", "DDS:          bool");
	ini.setValue("other/comment12", "Registration: bool");
	ini.setValue("other/comment13", "ExtraEffect:  bool");
}

QSize GlobalConfigManager::imageSize() const
{
	int w = ini.value(
		GROUP_Image(KEY_Width),
		GetHardCodedDefaultValue(KEY_Width)
	).toInt();
	int h = ini.value(
		GROUP_Image(KEY_Height),
		GetHardCodedDefaultValue(KEY_Height)
	).toInt();
	return QSize{ w, h };
}

double GlobalConfigManager::scale() const
{
	return ini.value(
		GROUP_Portrait(KEY_Scale),
		GetHardCodedDefaultValue(KEY_Scale)
	).toDouble();
}
QPoint GlobalConfigManager::offset() const
{
	int x = ini.value(
		GROUP_Portrait(KEY_OffsetX),
		GetHardCodedDefaultValue(KEY_OffsetX)
	).toInt();
	int y = ini.value(
		GROUP_Portrait(KEY_OffsetY),
		GetHardCodedDefaultValue(KEY_OffsetY)
	).toInt();
	return QPoint{ x, y };
}
bool GlobalConfigManager::useForSpecies() const
{
	return ini.value(
		GROUP_Portrait(KEY_UseForSpecies),
		GetHardCodedDefaultValue(KEY_UseForSpecies)
	).toBool();
}
bool GlobalConfigManager::useForLeaders() const
{
	return ini.value(
		GROUP_Portrait(KEY_UseForLeaders),
		GetHardCodedDefaultValue(KEY_UseForLeaders)
	).toBool();
}
bool GlobalConfigManager::useForRulers() const
{
	return ini.value(
		GROUP_Portrait(KEY_UseForRulers),
		GetHardCodedDefaultValue(KEY_UseForRulers)
	).toBool();
}

QString GlobalConfigManager::script() const
{
	return ini.value(
		GROUP_Export(KEY_Script),
		GetHardCodedDefaultValue(KEY_Script)
	).toString();
}
bool GlobalConfigManager::exportDDS() const
{
	return ini.value(
		GROUP_Export(KEY_DDS),
		GetHardCodedDefaultValue(KEY_DDS)
	).toBool();
}
bool GlobalConfigManager::exportRegistration() const
{
	return ini.value(
		GROUP_Export(KEY_Registration),
		GetHardCodedDefaultValue(KEY_Registration)
	).toBool();
}
bool GlobalConfigManager::exportExtraEffect() const
{
	return ini.value(
		GROUP_Export(KEY_ExtraEffect),
		GetHardCodedDefaultValue(KEY_ExtraEffect)
	).toBool();
}


void GlobalConfigManager::setImageSize(const QSize& size)
{
	ini.setValue(GROUP_Image(KEY_Width),  size.width());
	ini.setValue(GROUP_Image(KEY_Height), size.height());
}

void GlobalConfigManager::setScale(double scale)
{
	ini.setValue(GROUP_Portrait(KEY_Scale), scale);
}
void GlobalConfigManager::setOffset(const QPoint& offset)
{
	ini.setValue(GROUP_Portrait(KEY_OffsetX), offset.x());
	ini.setValue(GROUP_Portrait(KEY_OffsetY), offset.y());
}
void GlobalConfigManager::setUseForSpecies(bool value)
{
	ini.setValue(GROUP_Portrait(KEY_UseForSpecies), value);
}
void GlobalConfigManager::setUseForLeaders(bool value)
{
	ini.setValue(GROUP_Portrait(KEY_UseForLeaders), value);
}
void GlobalConfigManager::setUseForRulers(bool value)
{
	ini.setValue(GROUP_Portrait(KEY_UseForRulers), value);
}

void GlobalConfigManager::setScript(const QString& script)
{
	ini.setValue(GROUP_Export(KEY_Script), script);
}
void GlobalConfigManager::setExportDDS(bool value)
{
	ini.setValue(GROUP_Export(KEY_DDS), value);
}
void GlobalConfigManager::setExportRegistration(bool value)
{
	ini.setValue(GROUP_Export(KEY_Registration), value);
}
void GlobalConfigManager::setExportExtraEffect(bool value)
{
	ini.setValue(GROUP_Export(KEY_ExtraEffect), value);
}
