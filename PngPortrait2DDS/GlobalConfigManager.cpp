#include "GlobalConfigManager.h"
#include <QDir>

GlobalConfigManager* GlobalConfigManager::manager = nullptr;

#define GROUP_Image(k) "Image/" k
#define KEY_Width  "Width"
#define KEY_Height "Height"

#define GROUP_Portrait(k) "Portrait/" k
#define KEY_Scale         "Scale"
#define KEY_OffsetX       "OffsetX"
#define KEY_OffsetY       "OffsetY"
#define KEY_UseForSpecies "UseForSpecies"
#define KEY_UseForLeaders "UseForLeaders"
#define KEY_UseForRulers  "UseForRulers"

#define GROUP_Export(k)  "Export/" k
#define KEY_DDS              "DDS"
#define KEY_Registration     "Registration"
#define KEY_ProperNameEffect "ProperNameEffect"

const char* default_ini_file_content =
"[other]\n"
"comment1 = \"this is config of Portrait2DDS program, which defines the default value of some properties\"\n"
"commnet2 = \"value types:\"\n"
"commnet3 = \"Width:   int\"\n"
"commnet4 = \"Height:  int\"\n"
"commnet5 = \"Scale:   double\"\n"
"commnet6 = \"OffsetX: int\"\n"
"commnet7 = \"OffsetY: int\"\n"
"commnet8 = \"UseForSpecies: bool\"\n"
"commnet9 = \"UseForLeaders: bool\"\n"
"commnet10 = \"UseForRulers: bool\"\n"
"commnet11 = \"DDS:              bool\"\n"
"commnet12 = \"Registration:     bool\"\n"
"commnet13 = \"ProperNameEffect: bool\"\n"
"\n[Image]\n"
"Width  = 280\n"
"Height = 160\n"
"\n[Portrait]"
"Scale = 0.20\n"
"OffsetX = 0\n"
"OffsetY = 0\n"
"UseForSpecies = 1\n"
"UseForLeaders = 1\n"
"UseForRulers  = 1\n"
"\n[Export]\n"
"DDS              = 1\n"
"Registration     = 0\n"
"ProperNameEffect = 0\n";


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

	if (!ini.contains(GROUP_Export(KEY_DDS)) ||
		(ini.value(GROUP_Export(KEY_DDS)).type() != QVariant::Bool))
		ini.setValue(GROUP_Export(KEY_DDS), true);

	if (!ini.contains(GROUP_Export(KEY_Registration)) ||
		(ini.value(GROUP_Export(KEY_Registration)).type() != QVariant::Bool))
		ini.setValue(GROUP_Export(KEY_Registration), false);

	if (!ini.contains(GROUP_Export(KEY_ProperNameEffect)) ||
		(ini.value(GROUP_Export(KEY_ProperNameEffect)).type() != QVariant::Bool))
		ini.setValue(GROUP_Export(KEY_ProperNameEffect), false);


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
	ini.setValue("other/comment10", "DDS:              bool");
	ini.setValue("other/comment11", "Registration:     bool");
	ini.setValue("other/comment12", "ProperNameEffect: bool");
}

QSize GlobalConfigManager::imageSize() const
{
	int w = ini.value(GROUP_Image(KEY_Width),  280).toInt();
	int h = ini.value(GROUP_Image(KEY_Height), 160).toInt();
	return QSize{ w, h };
}

double GlobalConfigManager::scale() const
{
	return ini.value(GROUP_Portrait(KEY_Scale), 0.20).toDouble();
}
QPoint GlobalConfigManager::offset() const
{
	int x = ini.value(GROUP_Portrait(KEY_OffsetX), 0).toInt();
	int y = ini.value(GROUP_Portrait(KEY_OffsetY), 0).toInt();
	return QPoint{ x, y };
}
bool GlobalConfigManager::useForSpecies() const
{
	return ini.value(GROUP_Portrait(KEY_UseForSpecies), true).toBool();
}
bool GlobalConfigManager::useForLeaders() const
{
	return ini.value(GROUP_Portrait(KEY_UseForLeaders), true).toBool();
}
bool GlobalConfigManager::useForRulers() const
{
	return ini.value(GROUP_Portrait(KEY_UseForRulers), true).toBool();
}

bool GlobalConfigManager::exportDDS() const
{
	return ini.value(GROUP_Export(KEY_DDS), true).toBool();
}
bool GlobalConfigManager::exportRegistration() const
{
	return ini.value(GROUP_Export(KEY_Registration), true).toBool();
}
bool GlobalConfigManager::exportProperNameEffect() const
{
	return ini.value(GROUP_Export(KEY_ProperNameEffect), true).toBool();
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

void GlobalConfigManager::setExportDDS(bool value)
{
	ini.setValue(GROUP_Export(KEY_DDS), value);
}
void GlobalConfigManager::setExportRegistration(bool value)
{
	ini.setValue(GROUP_Export(KEY_Registration), value);
}
void GlobalConfigManager::setExportProperNameEffect(bool value)
{
	ini.setValue(GROUP_Export(KEY_ProperNameEffect), value);
}
