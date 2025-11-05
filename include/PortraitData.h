#pragma once

#include <QString>
#include <QPoint>

#include "utils/QtFileSystem.h"
#include "utils/misc.h"

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

	bool containsDirectory(const QtFileSystem::Path& path) const;

	static constexpr struct tag_no_except{} no_except_tag{};

	const_reference at(const QtFileSystem::Path& path) const;
	reference at(const QtFileSystem::Path& path);

	optional_const_reference at(const QtFileSystem::Path& path, tag_no_except tag) const;
	optional_reference at(const QtFileSystem::Path& path, tag_no_except tag);

	reference addDirectory(const QtFileSystem::Path& path);
	iterator  removeDirectory(const QtFileSystem::Path& path);

	void save(const QtFileSystem::Path& path) const;

private:
	PortraitDirectoryList data;

public:
	optional_const_reference operator[](const QtFileSystem::Path& dir) const
	{ return at(dir, no_except_tag); }
	optional_reference operator[](const QtFileSystem::Path& dir)
	{ return at(dir, no_except_tag); }
};

class FlatPortraitView
{
public:
	struct PortraitItemView
	{
		PortraitManager::iterator dir;
		PortraitDirectory::iterator p;

		Portrait* operator->() { return &*p; }
		Portrait& operator*()  { return *p; }

		QtFileSystem::Path path() const
		{
			return dir->path / p->filename;
		}
	};
	struct iterator
	{
		using iterator_category = std::forward_iterator_tag;
		using value_type = PortraitItemView;

		PortraitItemView operator*() { return { dir, p }; }

		iterator& operator++()
		{
			++p;
			if (p == dir->end())
			{
				++dir;
				p = dir == view->dir_end ? PortraitDirectory::iterator{} : dir->begin();
			}
			return *this;
		}
		iterator& operator--()
		{
			if (p == dir->begin())
			{
				--dir;
				p = dir->end();
			}
			--p;
			return *this;
		}

		bool operator==(const iterator& other) const
		{
			return dir == other.dir && p == other.p;
		}
		bool operator<(const iterator& other) const
		{
			if (dir <  other.dir) return true;
			if (dir == other.dir) return p < other.p;
			return false;
		}
		bool operator>(const iterator& other) const
		{
			if (dir >  other.dir) return true;
			if (dir == other.dir) return p > other.p;
			return false;
		}
		bool operator<=(const iterator& other) const
		{ return this->operator==(other) || this->operator<(other); }
		bool operator>=(const iterator& other) const
		{ return this->operator==(other) || this->operator>(other); }

		std::ptrdiff_t operator-(const iterator& rhs) const;
		iterator& operator-=(std::ptrdiff_t distance);
		iterator& operator+=(std::ptrdiff_t distance);

		PortraitManager::iterator dir;
		PortraitDirectory::iterator p;

		FlatPortraitView* view;
	};
	using value_type = PortraitItemView;
	using const_iterator = iterator;
public:
	FlatPortraitView(PortraitManager& m)
		: m{ &m }, dir_end{ m.end() } {}
	FlatPortraitView(const FlatPortraitView&) = default;

	// technically a default constructor is not needed, but QtConcurrent functions requires this
	FlatPortraitView() : m{ nullptr } {}

	iterator begin()
	{
		auto dir_begin = m->begin();
		if (dir_begin == dir_end) return end();
		return { dir_begin, m->begin()->begin(), this };
	}
	iterator end() { return { m->end(), PortraitDirectory::iterator{}, this }; }

	const_iterator cbegin()
	{
		auto dir_begin = m->begin();
		if (dir_begin == dir_end) return end();
		return { dir_begin, m->begin()->begin(), this };
	}
	const_iterator cend() { return { m->end(), PortraitDirectory::iterator{}, this }; }

	std::size_t size() const
	{
		auto ths = const_cast<FlatPortraitView*>(this);
		return ths->end() - ths->begin();
	}
private:
	friend class iterator;

	PortraitManager* m;
	PortraitManager::iterator dir_end;
};


template<>
struct std::iterator_traits<FlatPortraitView::iterator> : std::iterator_traits<PortraitManager::iterator>{};
