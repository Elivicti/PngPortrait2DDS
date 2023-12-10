#pragma once
#include <QSettings>
#include <QPoint>
#include <QSize>

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

	bool exportDDS() const;
	bool exportRegistration() const;
	bool exportProperNameEffect() const;


	void setImageSize(const QSize& size);

	void setScale(double scale);
	void setOffset(const QPoint& offset);

	void setUseForSpecies(bool value);
	void setUseForLeaders(bool value);
	void setUseForRulers(bool value);

	void setExportDDS(bool value);
	void setExportRegistration(bool value);
	void setExportProperNameEffect(bool value);

private:
	GlobalConfigManager();
	void defaultValueCheck();
	void commentCheck();

private:
	static GlobalConfigManager* manager;
	
	QSettings ini;

};

#define gConfig (GlobalConfigManager::instance())
