#pragma once
#include <QSettings>
#include <QPoint>
#include <QSize>

// GROUP: Image
#define KEY_Width  "Width"
#define KEY_Height "Height"
// GROUP: Portrait
#define KEY_Scale         "Scale"
#define KEY_OffsetX       "OffsetX"
#define KEY_OffsetY       "OffsetY"
#define KEY_UseForSpecies "UseForSpecies"
#define KEY_UseForLeaders "UseForLeaders"
#define KEY_UseForRulers  "UseForRulers"
// GROUP: Export
#define KEY_Script           "Script"
#define KEY_DDS              "DDS"
#define KEY_Registration     "Registration"
#define KEY_ExtraEffect "ExtraEffect"

struct HardCodedDefault
{
	static const int H_KEY_Width;
	static const int H_KEY_Height;
	static const double H_KEY_Scale;
	static const int H_KEY_OffsetX;
	static const int H_KEY_OffsetY;
	static const bool H_KEY_UseForSpecies;
	static const bool H_KEY_UseForLeaders;
	static const bool H_KEY_UseForRulers;
	static const char* H_KEY_Script;
	static const bool H_KEY_DDS;
	static const bool H_KEY_Registration;
	static const bool H_KEY_ExtraEffect;
};

#define GetHardCodedDefaultValue(key) HardCodedDefault::H_##key

class GlobalConfigManager
{
public:
	static void init()
	{
		if (manager == nullptr)
			manager = new GlobalConfigManager{};
	}
	static GlobalConfigManager* instance() { return manager; }
	static void release() { delete manager; manager = nullptr; }

	QSize imageSize() const;

	double scale() const;
	QPoint offset() const;

	bool useForSpecies() const;
	bool useForLeaders() const;
	bool useForRulers() const;

	QString script() const;
	bool exportDDS() const;
	bool exportRegistration() const;
	bool exportExtraEffect() const;


	void setImageSize(const QSize& size);

	void setScale(double scale);
	void setOffset(const QPoint& offset);

	void setUseForSpecies(bool value);
	void setUseForLeaders(bool value);
	void setUseForRulers(bool value);

	void setScript(const QString& script);
	void setExportDDS(bool value);
	void setExportRegistration(bool value);
	void setExportExtraEffect(bool value);

	void sync() { ini.sync(); }

private:
	GlobalConfigManager();
	void defaultValueCheck();
	void commentCheck();

private:
	static GlobalConfigManager* manager;
	
	QSettings ini;

};

#define gConfig (GlobalConfigManager::instance())
