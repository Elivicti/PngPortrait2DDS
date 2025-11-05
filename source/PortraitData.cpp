#include "PortraitData.h"
#include "Settings.h"

#if defined(_WIN32)
using QStringFilenameEqual   = CaseInsensitiveComparer<std::equal_to<int>>;
using QStringFilenameLess    = CaseInsensitiveComparer<std::less<int>>;
using QStringFilenameGreater = CaseInsensitiveComparer<std::greater<int>>;
#else
using QStringFilenameEqual   = std::equal_to<QString>;
using QStringFilenameLess    = std::less<QString>;
using QStringFilenameGreater = std::greater<QString>;
#endif

static constexpr auto qstring_filename_equal   = QStringFilenameEqual{};
static constexpr auto qstring_filename_less    = QStringFilenameLess{};
static constexpr auto qstring_filename_greater = QStringFilenameGreater{};

// ----------------------------------------------------------

#include <SimpleReflect/Reflect.hpp>
#include <QJsonObject>
#include <QJsonArray>

REFLECT_DEFINE_GLOBAL(PortraitConfig) {
	REFLECT_DEFINE() {
		REFLECT_MEMBER(offset),
		REFLECT_MEMBER(scale),
	};
};


REFLECT_DEFINE_GLOBAL(Portrait) {
	REFLECT_DEFINE() {
		REFLECT_MEMBER(filename),
		REFLECT_MEMBER(config),
		REFLECT_MEMBER(enabled),
	};
};

REFLECT_DEFINE_GLOBAL(PortraitDirectory) {
	REFLECT_DEFINE() {
		REFLECT_MEMBER(path),
		REFLECT_MEMBER(contents),
	};
};

#if defined(_WIN32) && USE_WCHAR
#define TO_QSTRING(str) QStringView{ str }
#else
#define TO_QSTRING(str) QString::fromUtf8(str)
#endif

struct PortraitConfig_serializer
{
	QJsonObject& obj;
	void operator()(const PortraitConfig*, Reflect::StringView name, const QPoint& offset)
	{
		QJsonObject offset_obj;
		offset_obj.insert("x", offset.x());
		offset_obj.insert("y", offset.y());

		obj.insert(TO_QSTRING(name), offset_obj);
	}

	template<typename T>
		requires std::is_fundamental_v<T>
	void operator()(const PortraitConfig*, Reflect::StringView name, T value)
	{
		obj.insert(TO_QSTRING(name), value);
	}
};

struct PortraitConfig_deserializer
{
	const QJsonObject& obj;
	void operator()(PortraitConfig*, Reflect::StringView name, QPoint& offset) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname))
			return;

		QJsonObject offset_obj{ obj.value(qname).toObject() };
		if (offset_obj.contains("x"))
			offset.setX(offset_obj.value("x").toInt());
		if (offset_obj.contains("y"))
			offset.setY(offset_obj.value("y").toInt());
	}

	void operator()(PortraitConfig*, Reflect::StringView name, double& scale) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname))
			return;
		QJsonValue scale_val{ obj.value(qname) };
		scale = scale_val.toDouble();
	}
};


struct Portrait_serializer
{
	QJsonObject& obj;

	void operator()(const Portrait*, Reflect::StringView name, const QString& filename)
	{
		obj.insert(TO_QSTRING(name), filename);
	}
	template<typename T>
		requires std::is_fundamental_v<T>
	void operator()(const Portrait*, Reflect::StringView name, T value)
	{
		obj.insert(TO_QSTRING(name), value);
	}

	void operator()(const Portrait*, Reflect::StringView name, const PortraitConfig& config)
	{
		QJsonObject config_obj;
		Reflect::for_each_member(&config, PortraitConfig_serializer{ config_obj });
		obj.insert(TO_QSTRING(name), config_obj);
	}
};

struct Portrait_deserializer
{
	const QJsonObject& obj;

	void operator()(Portrait*, Reflect::StringView name, QString& filename) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname)) return;
		filename = obj.value(qname).toString();
	}
	void operator()(Portrait*, Reflect::StringView name, bool& enabled) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname)) return;
		enabled = obj.value(qname).toBool();
	}

	void operator()(Portrait*, Reflect::StringView name, PortraitConfig& config) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname)) return;

		QJsonObject config_obj{ obj.value(qname).toObject() };
		Reflect::for_each_member(&config, PortraitConfig_deserializer{ config_obj });
	}
};


struct PortraitDirectory_serializer
{
	QJsonObject& obj;

	void operator()(const PortraitDirectory*, Reflect::StringView name, const QtFileSystem::Path& path)
	{
		obj.insert(TO_QSTRING(name), path.generic_qstring());
	}
	void operator()(const PortraitDirectory*, Reflect::StringView name, const QList<Portrait>& contents)
	{
		QJsonArray arr;
		for (auto& p : contents)
		{
			QJsonObject portrait_obj;
			Reflect::for_each_member(&p, Portrait_serializer{ portrait_obj });
			arr.append(portrait_obj);
		}
		obj.insert(TO_QSTRING(name), arr);
	}
};
struct PortraitDirectory_deserializer
{
	const QJsonObject& obj;

	void operator()(PortraitDirectory*, Reflect::StringView name, QtFileSystem::Path& path) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname))
			return;

		path = obj.value(qname).toString();
	}
	void operator()(PortraitDirectory*, Reflect::StringView name, QList<Portrait>& contents) const
	{
		auto qname = TO_QSTRING(name);
		if (!obj.contains(qname))
			return;
		std::set<QString, QStringFilenameLess> seen;
		for (auto var : obj.value(qname).toArray())
		{
			Portrait p;
			Reflect::for_each_member(&p, Portrait_deserializer{ var.toObject() });
			if (seen.contains(p.filename))
				continue;
			seen.insert(p.filename);
			contents.emplaceBack(std::move(p));
		}
	}
};

// ----------------------------------------------------------


PortraitDirectory::const_reference PortraitDirectory::at(const QString& filename) const
{
	if (auto var = at(path, no_except_tag))
		return var.value();
	throw std::invalid_argument{ "directory not found" };
}
PortraitDirectory::reference PortraitDirectory::at(const QString& filename)
{
	if (auto var = at(path, no_except_tag))
		return var.value();
	throw std::invalid_argument{ "directory not found" };
}

PortraitDirectory::optional_const_reference PortraitDirectory::at(const QString& filename, tag_no_except) const
{
	auto it = std::ranges::find_if(contents, [&filename](const Portrait& p) {
		return qstring_filename_equal(p.filename, filename);
	});

	if (it == contents.end())
		return std::nullopt;

	return *it;
}
PortraitDirectory::optional_reference PortraitDirectory::at(const QString& filename, tag_no_except)
{
	auto it = std::ranges::find_if(contents, [&filename](const Portrait& p) {
		return qstring_filename_equal(p.filename, filename);
	});

	if (it == contents.end())
		return std::nullopt;

	return *it;
}

void PortraitDirectory::refresh()
{
	if (!QtFileSystem::is_directory(path))
		return;

	for (const auto& entry : QtFileSystem::DirectoryEntryList{ path, { "*.png" }, QDir::Files })
	{
		QString filename{ entry.path().filename() };
		auto pred = [&filename](const Portrait& p) {
			return qstring_filename_equal(p.filename, filename);
		};

		if (auto it = std::ranges::find_if(contents, pred); it != contents.end())
			continue; // already inside list

		contents.emplaceBack(Portrait{
			.filename=filename,
			.config={},
			.enabled=true,
		});
	}
}
void PortraitDirectory::refresh(PortraitDirectory::tag_remove_missing_pictures)
{
	if (!QtFileSystem::is_directory(path))
	{
		contents.clear();
		return;
	}

	std::set<QString, QStringFilenameLess> seen;

	for (const auto& entry : QtFileSystem::DirectoryEntryList{ path, { "*.png" }, QDir::Files })
	{
		QString filename{ entry.path().filename() };
		auto pred = [&filename](const Portrait& p) {
			return qstring_filename_equal(p.filename, filename);
		};
		seen.emplace(filename);

		if (auto it = std::ranges::find_if(contents, pred); it != contents.end())
			continue; // already inside list

		contents.emplaceBack(Portrait{
			.filename=filename,
			.config={},
			.enabled=true,
		});
	}

	auto end = contents.end();
	for (auto it = contents.begin(); it != end; ++it)
	{
		if (seen.contains(it->filename))
			continue;

		it = contents.erase(it);
		end = contents.end();
	}
}

// ----------------------------------------------------------
#include <QJsonDocument>
#include <QFile>

PortraitManager::PortraitManager(const QtFileSystem::Path& preset)
{
	if (!QtFileSystem::is_regular_file(preset) || preset.extension() != ".json")
		throw std::invalid_argument{ "input path is not a file or is not json file" };


	QFile file{ preset };
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		throw std::runtime_error{ file.errorString().toStdString() };
		return;
	}

	QJsonDocument json_doc{ QJsonDocument::fromJson(file.readAll()) };

	if (!json_doc.isArray())
		throw std::runtime_error{ "invalid preset json" };

	std::set<QtFileSystem::Path> seen;
	for (auto var : json_doc.array())
	{
		if (!var.isObject())
			continue; // TODO: print some log

		PortraitDirectory dir;
		Reflect::for_each_member(&dir, PortraitDirectory_deserializer{ var.toObject() });

		if (dir.path.empty())
			continue; // TODO: print some log

		if (!dir.path.is_absolute())
			dir.path = QtFileSystem::absolute(dir.path);

		if (seen.contains(dir.path))
			continue; // TODO: print some log

		seen.insert(dir.path);
		data.emplaceBack(std::move(dir));
	}
}

bool PortraitManager::containsDirectory(const QtFileSystem::Path& path) const
{
	return std::ranges::find_if(data, [&path](const PortraitDirectory& dir) {
		return QtFileSystem::absolute(path) == dir.path;
	}) != data.end();
}

PortraitManager::reference PortraitManager::addDirectory(const QtFileSystem::Path& path)
{
	if (!QtFileSystem::is_directory(path))
		throw std::invalid_argument{ "input path is not a directory" };

	if (auto dir = this->at(path, no_except_tag))
	{
		dir->get().refresh();
		return dir.value();
	}

	auto& settings = SettingsManager::instance().settings();

	QList<Portrait> portraits;
	for (auto entry : QtFileSystem::DirectoryEntryList{ path, { "*.png" } })
	{
		portraits.emplaceBack(Portrait{
			.filename = entry.path().filename(),
			.config = {
				.offset = settings.default_offset,
				.scale  = settings.default_scale,
			},
			.enabled = true,
		});
	}
	return data.emplaceBack(QtFileSystem::absolute(path), std::move(portraits));
}
PortraitManager::iterator PortraitManager::removeDirectory(const QtFileSystem::Path& path)
{
	auto it = std::ranges::find_if(data, [&path](const PortraitDirectory& d) {
		return d.path == QtFileSystem::absolute(path);
	});
	return data.erase(it);
}

PortraitManager::const_reference PortraitManager::at(const QtFileSystem::Path& path) const
{
	if (auto var = at(path, no_except_tag))
		return var.value();
	throw std::invalid_argument{ "directory not found" };
}
PortraitManager::reference PortraitManager::at(const QtFileSystem::Path& path)
{
	if (auto var = at(path, no_except_tag))
		return var.value();
	throw std::invalid_argument{ "directory not found" };
}

PortraitManager::optional_const_reference PortraitManager::at(const QtFileSystem::Path& path, tag_no_except tag) const
{
	auto it = std::ranges::find_if(data, [&path](const PortraitDirectory& dir) {
		return QtFileSystem::absolute(path) == dir.path;
	});

	if (it == data.end())
		return std::nullopt;

	return *it;
}
PortraitManager::optional_reference PortraitManager::at(const QtFileSystem::Path& path, tag_no_except tag)
{
	auto it = std::ranges::find_if(data, [&path](const PortraitDirectory& dir) {
		return QtFileSystem::absolute(path) == dir.path;
	});

	if (it == data.end())
		return std::nullopt;

	return *it;
}


void PortraitManager::save(const QtFileSystem::Path& path) const
{
	if (QtFileSystem::is_directory(path))
		throw std::invalid_argument{ "input path is a directory" };

	if (QtFileSystem::is_regular_file(path) && path.extension() != ".json")
		throw std::invalid_argument{ "input path exists and is not json file" };


	if (this->size() == 0)
		return;

	QJsonArray arr;

	for (auto& dir : data)
	{
		QJsonObject dir_obj;
		Reflect::for_each_member(&dir, PortraitDirectory_serializer{ dir_obj });
		arr.append(std::move(dir_obj));
	}

	QFile f{ path };
	if (!f.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
	{
		throw std::runtime_error{ f.errorString().toStdString() };
		return;
	}
	f.write(QJsonDocument{ arr }.toJson());
}

// ----------------------------------------------------------

std::ptrdiff_t FlatPortraitView::iterator::operator-(const iterator& rhs) const
{
	if (dir == rhs.dir) return p - rhs.p;

	std::ptrdiff_t distance = 0;

	auto [min, max] = std::pair<iterator, iterator>{ std::minmax(*this, rhs) };
	distance -= min.p - min.dir->begin();
	if (max.dir != max.view->dir_end)
		distance += max.p - max.dir->begin();

	while (min.dir != max.dir)
	{
		distance += min.dir->end() - min.dir->begin();
		++min.dir;
	}

	std::ptrdiff_t sign = dir < rhs.dir ? -1 : 1;
	return distance * sign;
}

FlatPortraitView::iterator& FlatPortraitView::iterator::operator-=(std::ptrdiff_t distance)
{
	return *this += -distance;
}

FlatPortraitView::iterator& FlatPortraitView::iterator::operator+=(std::ptrdiff_t distance)
{
	while (distance != 0)
	{
		auto dir_size = dir->size();
		auto idx = p - dir->begin();
		if (distance > 0)
		{
			auto remain = dir_size - idx;
			if (distance < remain)
			{
				p += distance;
				break;
			}
			distance -= remain;
			++dir;
			if (dir == view->dir_end)
			{
				p = PortraitDirectory::iterator{};
				break;
			}
			p = dir->begin();
		}
		else
		{
			if (-distance <= idx)
			{
				p += distance;
				break;
			}
			distance += idx;
			if (dir == view->m->begin())
			{
				p = dir->begin();
				break;
			}
			--dir;
			p = dir->end();
		}
	}
	return *this;
}
