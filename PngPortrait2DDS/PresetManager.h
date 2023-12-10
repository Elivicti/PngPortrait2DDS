#pragma once
#include <QJsonDocument>
#include <QFileInfo>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QList>
#include <QMap>

class PresetData;
class PortraitsData;

class PresetSaver
{
public:

	bool savePreset(const PresetData& preset, const QString& filepath);

private:

};

class PresetLoader
{
public:
	PresetLoader() : load_no_error{ false }, error_msg{} {}

	std::pair<PresetData, QJsonParseError> loadFromJson(const QString& filepath);

	PresetData loadFromDirectory(const QString& path);

	bool success() const { return load_no_error; }
	const QString& errorString() const { return error_msg; }

private:
	bool load_no_error;
	QString error_msg;
};

class PresetData
{
public:
	struct PortraitUsageTypes
	{
		bool species;
		bool leaders;
		bool rulers;
	};
	using SharedDataPointer = std::shared_ptr<PortraitsData>;
public:
	PresetData(const QSize& image_size, const QPoint& defualt_offset, double default_scale);

	const QString& directory() const { return path; }
	/** @param "name" refers to the name of the png file, with suffix(.png) but not with path */
	PortraitUsageTypes usage(const QString& name) const { return types.value(name); }
	/** @param "name" refers to the name of the png file, with suffix(.png) but not with path */
	PortraitUsageTypes& usage(const QString& name);

	const QSize& imageSize() const { return img_size; }
	QSize& imageSize() { return img_size; }

	bool exportDDS()              const { return export_options.dds; }
	bool exportRegistration()     const { return export_options.registration; }
	bool exportProperNameEffect() const { return export_options.proper_name_effect; }

	bool& exportDDS()              { return export_options.dds; }
	bool& exportRegistration()     { return export_options.registration; }
	bool& exportProperNameEffect() { return export_options.proper_name_effect; }

	SharedDataPointer portraitData() const { return portraits; }

private:
	QString path;
	QSize img_size;
	struct {
		bool dds;
		bool registration;
		bool proper_name_effect;
	} export_options;
	QMap<QString, PortraitUsageTypes> types;

	friend class PresetSaver;
	friend class PresetLoader;

	std::shared_ptr<PortraitsData> portraits;

	PresetData()
		: path{}, img_size{}
		, export_options{ true, false, false }, types{}
		, portraits{ std::make_shared<PortraitsData>() } {}

	PresetData(
		const QString& path, const QSize& img_size,
		bool export_dds, bool export_registration, bool export_proper_name_effect
	) : path{ path }, img_size{ img_size }
	  , export_options{ export_dds, export_registration, export_proper_name_effect }
	  , types{} , portraits{ std::make_shared<PortraitsData>() } {}

public:
	PortraitsData* operator->() const { return portraits.operator->(); }
};

class PortraitsData
{
public:
	struct Portrait
	{
		QFileInfo portrait;

		bool independent_setting;
		QPoint offset;
		double scale;

		QString fileName() const { return portrait.fileName(); }
		QString baseName() const { return portrait.baseName(); }
		QString absoluteFilePath() const { return portrait.absoluteFilePath(); }

		// only compares file name, with suffix but no path
		bool operator==(const QString& name) const
		{ return portrait.fileName() == name; }

		bool operator==(const QFileInfo& file) const
		{ return portrait == file; }

	};
	using PortraitList = QList<Portrait>;

public:

	PortraitList::const_iterator begin() const { return data.cbegin(); }
	PortraitList::const_iterator end() const { return data.cend(); }

	PortraitList::iterator begin() { return data.begin(); }
	PortraitList::iterator end() { return data.end(); }

	const QPoint& defaultOffset() const { return global.offset; }
	double defaultScale() const { return global.scale; }

	const QPoint& offset(qsizetype i) const
	{
		auto& portrait = data.at(i);
		return portrait.independent_setting ? portrait.offset : global.offset;
	}
	double scale(qsizetype i) const
	{
		auto& portrait = data.at(i);
		return portrait.independent_setting ? portrait.scale : global.scale;
	}

	bool useIndependentSettings(qsizetype i) const { return data.at(i).independent_setting; }

	void setDefaultOffset(const QPoint& off) { global.offset = off; }
	void setDefaultOffsetX(int x) { global.offset.setX(x); }
	void setDefaultOffsetY(int y) { global.offset.setY(y); }
	void setDefaultScale (const double& scl) { global.scale  = scl; }

	void setOffset(qsizetype i, const QPoint& off)
	{
		Portrait& portrait = data[i];
		portrait.offset = off;
		portrait.independent_setting = true;
	}
	void setOffsetX(qsizetype i, int x)
	{
		Portrait& portrait = data[i];
		portrait.offset.setX(x);
		portrait.independent_setting = true;
	}
	void setOffsetY(qsizetype i, int y)
	{
		Portrait& portrait = data[i];
		portrait.offset.setY(y);
		portrait.independent_setting = true;
	}
	void setScale(qsizetype i, const double& scl)
	{
		Portrait& portrait = data[i];
		portrait.scale = scl;
		portrait.independent_setting = true;
	}

	void setUseIndependentSettings(qsizetype i, bool independent)
	{ data[i].independent_setting = independent; }

	const Portrait& at(qsizetype i) { return data.at(i); }

	/** @param "name" refers to the name of the png file, with suffix(.png) but not with path */
	qsizetype indexOf(const QString& name) const
	{
		return data.indexOf(name);
	}

	/** @param "name" refers to the name of the png file, with suffix(.png) but not with path */
	Portrait& find(const QString& name)
	{
		return data[data.indexOf(name)];
	}
	/** @param "name" refers to the name of the png file, with suffix(.png) but not with path */
	const Portrait& find(const QString& name) const
	{
		return data.at(data.indexOf(name));
	}

	qsizetype size() const { return data.size(); }

	PortraitsData() : global{ QPoint{}, 0.0 }, data{} {}
	PortraitsData(const QPoint& defualt_offset, double default_scale)
		: global{ defualt_offset, default_scale }, data{} {}

private:
	void append(const QFileInfo& portrait, QPoint offset, double scale)
	{
		data.append(Portrait{ portrait, true, offset, scale });
	}
	void append(const QFileInfo& portrait)
	{
		// use global offset & scale if not using independent setting
		data.append(Portrait{ portrait, false, global.offset, global.scale });
	}

	friend class PresetSaver;
	friend class PresetLoader;
private:
	struct {
		QPoint offset;
		double scale;
	} global;
	PortraitList data;
};