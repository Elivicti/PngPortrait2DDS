#include "PresetManager.h"
#include "GlobalConfigManager.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDir>

#include <QDebug>

#define JSON_K_Path               "Path"
#define JSON_K_Offset             "Offset"
#define JSON_K_Scale              "Scale"
#define JSON_K_ImageSize          "ImageSize"
#define JSON_K_DDS                "DDS"
#define JSON_K_Registration       "Registration"
#define JSON_K_ProperNameEffect   "ProperNameEffect"
#define JSON_K_Name               "Name"
#define JSON_K_Species            "Species"
#define JSON_K_Leaders            "Leaders"
#define JSON_K_Rulers             "Rulers"
#define JSON_K_Types              "Types"
#define JSON_K_Settings           "Settings"
#define JSON_K_DefaultSettings    "DefaultSettings"
#define JSON_K_ExportOptions      "ExportOptions"
#define JSON_K_Portraits          "Portraits"

bool PresetSaver::savePreset(const PresetData& preset, const QString& filepath)
{
	QFile json(filepath);
	if (!json.open(QIODevice::ReadWrite | QIODevice::Text | QIODeviceBase::Truncate))
		return false;

	QJsonObject obj;						// main json object
	obj.insert(JSON_K_Path, preset.path);	// directory path

	std::shared_ptr<PortraitsData> data = preset.portraits;

	QJsonObject default_settings;
	default_settings.insert(JSON_K_Offset,    QJsonArray{ data->defaultOffset().x(), data->defaultOffset().y() });
	default_settings.insert(JSON_K_Scale,     data->defaultScale());
	default_settings.insert(JSON_K_ImageSize, QJsonArray{ preset.img_size.width(), preset.img_size.height() });

	QJsonObject export_options;
	export_options.insert(JSON_K_DDS,			  preset.export_options.dds);
	export_options.insert(JSON_K_Registration,	  preset.export_options.registration);
	export_options.insert(JSON_K_ProperNameEffect, preset.export_options.proper_name_effect);

	QJsonArray portraits;					// portraits data array
	for (auto& portrait : data->data)
	{
		QString name{ portrait.portrait.fileName() };
		bool independent_setting = portrait.independent_setting;

		QJsonObject portrait_obj;
		portrait_obj.insert(JSON_K_Name, name);
		
		QJsonObject types;					// types(species, leader or ruler) that this portrait will be used
		PresetData::PortraitUsageTypes t{ preset.types.value(name) };
		types.insert(JSON_K_Species, t.species);
		types.insert(JSON_K_Leaders, t.leaders);
		types.insert(JSON_K_Rulers,  t.rulers);

		portrait_obj.insert(JSON_K_Types, types);

		if (independent_setting)
		{
			QJsonObject settings;			// independent portrait settings
			settings.insert(JSON_K_Offset, QJsonArray{ portrait.offset.x(), portrait.offset.y() });
			settings.insert(JSON_K_Scale,  portrait.scale);

			portrait_obj.insert(JSON_K_Settings, settings);
		}

		portraits.append(portrait_obj);
	}

	obj.insert(JSON_K_DefaultSettings, default_settings);
	obj.insert(JSON_K_ExportOptions,   export_options);
	obj.insert(JSON_K_Portraits,       portraits);

	json.write(QJsonDocument(obj).toJson());
	json.close();
	return true;
}

std::pair<PresetData, QJsonParseError> PresetLoader::loadFromJson(const QString& filepath)
{
	QFile jsonfile{ filepath };
	auto ret = std::make_pair<PresetData, QJsonParseError>(PresetData{}, QJsonParseError{});
	if (!jsonfile.open(QIODevice::Text | QIODevice::ReadOnly))
	{
		error_msg = QString{ "Failed to open %1." }.arg(filepath);
		return ret;
	}

	QJsonDocument json_doc{ QJsonDocument::fromJson(jsonfile.readAll(), &ret.second) };
	if (json_doc.isNull())
	{
		error_msg = ret.second.errorString();
		return ret;
	}

	auto& preset = ret.first;
	QJsonObject obj{ json_doc.object() };		// main json object
	if (obj.contains(JSON_K_Path))
		preset.path = obj.value(JSON_K_Path).toString();
	else
	{
		error_msg = "Preset missing \"Path\" data.";
		return ret;
	}

	QDir dir_path{ preset.path };
	if (!dir_path.exists())
	{
		error_msg = QString{ "Directory \"%1\" does not exist." }.arg(dir_path.absolutePath());
		return ret;
	}

	if (obj.contains(JSON_K_DefaultSettings))
	{
		QJsonObject default_setting{ obj.value(JSON_K_DefaultSettings).toObject() };
		if (default_setting.contains(JSON_K_Offset))
		{
			QJsonArray offset{ default_setting.value(JSON_K_Offset).toArray() };
			preset.portraits->setDefaultOffset(QPoint{ offset[0].toInt(), offset[1].toInt() });
		}
		else
			preset.portraits->setDefaultOffset(gConfig->offset());

		if (default_setting.contains(JSON_K_Scale))
			preset.portraits->setDefaultScale(default_setting.value(JSON_K_Scale).toDouble());
		else
			preset.portraits->setDefaultScale(gConfig->scale());

		if (default_setting.contains(JSON_K_ImageSize))
		{
			QJsonArray img_size{ default_setting.value(JSON_K_ImageSize).toArray() };
			preset.img_size.setWidth(img_size[0].toInt());
			preset.img_size.setHeight(img_size[1].toInt());
		}
		else
			preset.img_size = gConfig->imageSize();
	}
	else
	{
		preset.portraits->setDefaultOffset(gConfig->offset());
		preset.portraits->setDefaultScale(gConfig->scale());
		preset.img_size = gConfig->imageSize();
	}

	if (obj.contains(JSON_K_ExportOptions))
	{
		QJsonObject export_option{ obj.value(JSON_K_ExportOptions).toObject() };
		preset.export_options.dds =
			export_option.contains(JSON_K_DDS) ?
			export_option.value(JSON_K_DDS).toBool() : gConfig->exportDDS();

		preset.export_options.registration =
			export_option.contains(JSON_K_Registration) ?
			export_option.value(JSON_K_Registration).toBool() : gConfig->exportRegistration();

		preset.export_options.proper_name_effect =
			export_option.contains(JSON_K_ProperNameEffect) ?
			export_option.value(JSON_K_ProperNameEffect).toBool() : gConfig->exportProperNameEffect();
	}
	else
	{
		preset.export_options.dds = gConfig->exportDDS();
		preset.export_options.registration = gConfig->exportRegistration();
		preset.export_options.proper_name_effect = gConfig->exportProperNameEffect();
	}

	if (obj.contains(JSON_K_Portraits))
	{
		QJsonArray portraits_array{ obj.value(JSON_K_Portraits).toArray() };
		for (auto portrait_value : portraits_array)
		{
			QJsonObject portrait{ portrait_value.toObject() };
			QJsonValue name_value{ portrait.value(JSON_K_Name) };
			if (name_value.isUndefined() || !dir_path.exists(name_value.toString()))
				continue;

			QString name{ name_value.toString() };
			PresetData::PortraitUsageTypes types{ true, true, true };

			if (portrait.contains(JSON_K_Types))
			{
				QJsonObject types_value{ portrait.value(JSON_K_Types).toObject() };
				types.species = types_value.contains(JSON_K_Species) ?
					types_value.value(JSON_K_Species).toBool() : true;

				types.leaders = types_value.contains(JSON_K_Leaders) ?
					types_value.value(JSON_K_Leaders).toBool() : true;

				types.rulers = types_value.contains(JSON_K_Rulers) ?
					types_value.value(JSON_K_Rulers).toBool() : true;
			}
			preset.types.insert(name, types);

			bool independent_setting = portrait.contains(JSON_K_Settings);
			if (!independent_setting)
			{
				preset.portraits->append(QFileInfo{ dir_path.absoluteFilePath(name) });
				continue;
			}

			QJsonObject settings{ portrait.value(JSON_K_Settings).toObject() };

			QPoint offset{ gConfig->offset() };
			if (settings.contains(JSON_K_Offset))
			{
				QJsonArray offset_value{ settings.value(JSON_K_Offset).toArray() };
				offset.setX(offset_value[0].toInt());
				offset.setY(offset_value[1].toInt());
			}

			preset.portraits->append(
				QFileInfo{ dir_path.absoluteFilePath(name) },
				offset,
				settings.contains(JSON_K_Scale) ? settings.value(JSON_K_Scale).toDouble() : 0.20
			);
			
		}
	}
	else
	{
		// TODO: if "Portrait" entry (should be array type) does not exist, load from directory
		QFileInfoList png_files{ dir_path.entryInfoList(QStringList{ "*.png" }) };
		for (auto& png : png_files)
		{
			preset.portraits->append(png);
			preset.types.insert(png.fileName(), PresetData::PortraitUsageTypes{ true, true, true });
		}
	}

	jsonfile.close();
	load_no_error = true;
	return ret;
}

PresetData PresetLoader::loadFromDirectory(const QString& path)
{
	QDir dir_path{ path };
	if (!dir_path.exists())
	{
		error_msg = QString{ "Directory \"%1\" does not exist." }.arg(dir_path.absolutePath());
		return PresetData{};
	}

	PresetData preset{
		dir_path.absolutePath(),
		QSize{ 280,160 },
		true, false, false
	};

	preset->setDefaultScale(gConfig->scale());
	preset->setDefaultOffset(gConfig->offset());

	QFileInfoList png_files{ dir_path.entryInfoList(QStringList{ "*.png" }) };
	for (auto& png : png_files)
	{
		preset.portraits->append(png);
		preset.types.insert(png.fileName(), PresetData::PortraitUsageTypes{ true, true, true });
	}

	load_no_error = true;
	return preset;
}

PresetData::PresetData(const QSize& image_size, const QPoint& defualt_offset, double default_scale)
	: path{}, img_size{ image_size }
	, export_options{ gConfig->exportDDS(), gConfig->exportRegistration(), gConfig->exportProperNameEffect()}
	, types{}, portraits{ std::make_shared<PortraitsData>(defualt_offset, default_scale) }
{
}

PresetData::PortraitUsageTypes& PresetData::usage(const QString& name)
{
	static PortraitUsageTypes fallback{
		gConfig->useForSpecies(),
		gConfig->useForLeaders(),
		gConfig->useForRulers()
	};
	if (types.contains(name))
		return types[name];
	else
		return fallback;
}