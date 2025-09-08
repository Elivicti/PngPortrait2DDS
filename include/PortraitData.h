#pragma once

#include <QString>
#include <QPoint>

#include "utils/QtFileSystem.h"


template<typename Pred = std::less<int>>
struct CaseInsensitiveComparer
{
	bool operator()(const QString& lhs, const QString& rhs) const
	{
		return Pred{}(lhs.compare(rhs, Qt::CaseInsensitive), 0);
	}
};
template<typename T>
	requires (!std::is_reference_v<T>)
using optional_ref = std::optional<std::reference_wrapper<T>>;


struct PortraitConfig
{
	QPoint offset = { 0, 0 };
	double scale  = 1.0;


	bool operator==(const PortraitConfig& rhs) const
	{
		return offset == rhs.offset && scale == rhs.scale;
	}
};


struct Portrait
{
	QString filename;
	PortraitConfig config;
	bool enabled = true;

	auto operator==(const Portrait& rhs) const
	{ return filename == rhs.filename; }

	auto operator<=>(const Portrait& rhs) const
	{ return filename <=> rhs.filename; }
};

struct PortraitDirectory
{
	QtFileSystem::Path path;
	QList<Portrait> contents;

	using PortraitList = QList<Portrait>;

	using iterator = PortraitList::iterator;
	using const_iterator = PortraitList::const_iterator;

	using value_type = PortraitList::value_type;
	using reference = PortraitList::reference;
	using const_reference = PortraitList::const_reference;

	using optional_reference = optional_ref<value_type>;
	using optional_const_reference = optional_ref<const value_type>;

	using difference_type = PortraitList::difference_type;
	using size_type = PortraitList::size_type;

	iterator begin() noexcept { return contents.begin(); }
	iterator end()   noexcept { return contents.end(); }

	const_iterator begin()  const noexcept { return contents.begin(); }
	const_iterator end()    const noexcept { return contents.end(); }
	const_iterator cbegin() const noexcept { return contents.cbegin(); }
	const_iterator cend()   const noexcept { return contents.cend(); }

	size_type size() const noexcept { return contents.size(); }

	static constexpr struct tag_no_except{} no_except_tag{};

	const_reference at(const QString& filename) const;
	reference at(const QString& filename);

	optional_const_reference at(const QString& filename, tag_no_except tag) const;
	optional_reference at(const QString& filename, tag_no_except tag);

	static constexpr struct tag_remove_missing_pictures{} remove_missing_pictures_tag{};

	void refresh();
	void refresh(tag_remove_missing_pictures tag);
};

class PortraitManager
{
	using PortraitDirectoryList = QList<PortraitDirectory>;
public:
	using iterator = PortraitDirectoryList::iterator;
	using const_iterator = PortraitDirectoryList::const_iterator;

	using value_type = PortraitDirectoryList::value_type;
	using reference = PortraitDirectoryList::reference;
	using const_reference = PortraitDirectoryList::const_reference;

	using optional_reference = optional_ref<value_type>;
	using optional_const_reference = optional_ref<const value_type>;

	using difference_type = PortraitDirectoryList::difference_type;
	using size_type = PortraitDirectoryList::size_type;
public:
	PortraitManager() = default;
	PortraitManager(const QtFileSystem::Path& preset);

	iterator begin() noexcept { return data.begin(); }
	iterator end()   noexcept { return data.end(); }

	const_iterator begin()  const noexcept { return data.begin(); }
	const_iterator end()    const noexcept { return data.end(); }
	const_iterator cbegin() const noexcept { return data.cbegin(); }
	const_iterator cend()   const noexcept { return data.cend(); }

	size_type size() const noexcept { return data.size(); }

	static constexpr struct tag_no_except{} no_except_tag{};

	const_reference at(const QtFileSystem::Path& path) const;
	reference at(const QtFileSystem::Path& path);

	optional_const_reference at(const QtFileSystem::Path& path, tag_no_except tag) const;
	optional_reference at(const QtFileSystem::Path& path, tag_no_except tag);

	const_reference add_directory(const QtFileSystem::Path& path);
	const_iterator remove_directory(const QtFileSystem::Path& path);


	void save(const QtFileSystem::Path& path) const;

private:
	PortraitDirectoryList data;

public:
	optional_const_reference operator[](const QtFileSystem::Path& dir) const
	{ return at(dir, no_except_tag); }
	optional_reference operator[](const QtFileSystem::Path& dir)
	{ return at(dir, no_except_tag); }
};
