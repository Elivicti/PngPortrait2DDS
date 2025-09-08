#pragma once

#include <QString>
#include <QFileInfo>
#include <QDir>

#include <filesystem>

namespace QtFileSystem {

class Path
{
	using fspath = std::filesystem::path;
public:
	using value_type = fspath::value_type;
	using string_type = fspath::string_type;

	using const_iterator = fspath::const_iterator;
	using iterator = const_iterator;
public:
	Path() : path{} {}
	Path(const std::filesystem::path& path) : path{ path } {}
	Path(std::filesystem::path&& path)      : path{ std::move(path) } {}
	Path(const Path& other)   : path{ other.path } {}
	Path(Path&& other)        : path{ std::move(other.path) } {}
	Path(const QString& path) : path{ std::move(qstring_to_fspath(path)) } {}

	template<std::size_t N>
	Path(const char(& path)[N]) : path{ std::string_view{ path, N - 1 } } {}
	Path(const char* path)      : path{ path } {}

	template<std::size_t N>
	Path& operator=(const char(& path)[N])
	{
		this->path = std::string_view{ path, N };
		return *this;
	}
	Path& operator=(const Path& other)
	{
		if (this == &other)
			return *this;
		path = other.path;
		return *this;
	}
	Path& operator=(Path&& other)
	{
		if (this == &other)
			return *this;
		path.assign(other.path);
		return *this;
	}
	Path& operator=(const QString& path)
	{
		this->path.assign(qstring_to_fspath(path));
		return *this;
	}
	Path& operator=(const char* path)
	{
		this->path = path;
		return *this;
	}

	Path& append(const QString& s)
	{ path.append(qstring_to_fspath(s).string()); return *this; }
	template<typename Source>
	Path& append(const Source& s)
	{ path.append(s); return *this; }

	Path& operator/=(const Path& path)
	{ this->path /= path.path; return *this; }
	Path& operator/=(const QString& p)
	{ path /= qstring_to_fspath(p); return *this; }
	template<typename Source>
	Path& operator/=(const Source& s)
	{ path /= s; return *this; }


	Path& operator+=(const Path& p)
	{ path += p.path; return *this; }
	Path& operator+=(const string_type& str)
	{ path += str; return *this; }
	Path& operator+=(std::basic_string_view<value_type> str)
	{ path += str; return *this; }
	Path& operator+=(const value_type* ptr)
	{ path += ptr; return *this; }
	Path& operator+=(value_type x)
	{ path += x; return *this; }
	template<typename CharT>
	Path& operator+=(CharT x)
	{ path += x; return *this; }
	template<typename Source>
	Path& operator+=(const Source& source)
	{ path += source; return *this; }
	Path& operator+=(const QString& source)
	{ path += qstring_to_fspath(source); return *this; }

	template<typename Source>
	Path& concat(const Source& source)
	{ path.concat(source); return *this; }
	Path& concat(const QString& s)
	{ path.concat(qstring_to_fspath(s).string()); return *this; }


	void clear() noexcept { path.clear(); }

	Path& make_preferred()
	{ path.make_preferred(); return *this; }
	Path& remove_filename()
	{ path.remove_filename(); return *this; }
	Path& replace_filename(const Path& replacement)
	{ path.replace_filename(replacement.path); return *this; }
	Path& replace_extension(const Path& replacement = Path{})
	{ path.replace_extension(replacement.path); return *this; }
	Path& swap(Path& other)
	{ path.swap(other.path); return *this; }


	const value_type* c_str()   const { return path.c_str(); }
	const string_type& native() const { return path.native(); }
	operator string_type()      const { return path; }

	QString      qstring() const { return fspath_to_qstring(path); }
	std::string   string() const { return path.string();  }
	std::wstring wstring() const { return path.wstring(); }

	QString      generic_qstring() const { return fspath_to_generic_qstring(path); }
	std::string   generic_string() const { return path.generic_string();  }
	std::wstring generic_wstring() const { return path.generic_wstring(); }


	int compare(const Path& p) const noexcept { return path.compare(p.path); }
	int compare(std::basic_string_view<value_type> str) const { return path.compare(str); }
	int compare(const string_type& str)   const { return path.compare(str); }
	int compare(const value_type* s)      const { return path.compare(s); }
	int compare(const QString& s)         const { return path.compare(s.toStdString()); }

	Path lexically_normal() const { return path.lexically_normal(); }
	Path lexically_relative(const Path& base)  const { return path.lexically_relative(base.path);  }
	Path lexically_proximate(const Path& base) const { return path.lexically_proximate(base.path); }

	Path root_name()      const { return path.root_name(); }
	Path root_directory() const { return path.root_directory(); }
	Path root_path()      const { return path.root_path(); }
	Path relative_path()  const { return path.relative_path(); }
	Path parent_path()    const { return path.parent_path(); }
	Path filename()       const { return path.filename(); }
	Path stem()           const { return path.stem(); }
	Path extension()      const { return path.extension(); }

	bool empty()       const { return path.empty(); }
	bool is_absolute() const { return path.is_absolute(); }
	bool is_relative() const { return path.is_relative(); }

	bool has_root_name()      const { return path.has_root_name(); }
	bool has_root_directory() const { return path.has_root_directory(); }
	bool has_root_path()      const { return path.has_root_path(); }
	bool has_relative_path()  const { return path.has_relative_path(); }
	bool has_parent_path()    const { return path.has_parent_path(); }
	bool has_filename()       const { return path.has_filename(); }
	bool has_stem()           const { return path.has_stem(); }
	bool has_extension()      const { return path.has_extension(); }

	iterator begin() const { return path.begin(); }
	iterator   end() const { return path.end();   }

private:
	std::filesystem::path path;

	static std::filesystem::path qstring_to_fspath(const QString& s)
	{ return s.toStdU16String(); }
	static QString fspath_to_qstring(const std::filesystem::path& p)
	{ return QString::fromStdU16String(p.u16string()); }
	static QString fspath_to_generic_qstring(const std::filesystem::path& p)
	{ return QString::fromStdU16String(p.generic_u16string()); }

public:
	operator const std::filesystem::path&() const { return path; }
	operator QString() const { return generic_qstring(); }

	bool operator== (const Path& rhs) const noexcept { return path ==  rhs.path; }
	auto operator<=>(const Path& rhs) const noexcept { return path <=> rhs.path; }

	Path operator/(const Path& rhs) const { return path / rhs.path; }
};

inline Path current_path() { return std::filesystem::current_path(); }

#define STDFS_FUNC_DEFINE(ret, name) \
	inline ret name(const Path& p) { return std::filesystem::name(p); }

STDFS_FUNC_DEFINE(bool, exists)

STDFS_FUNC_DEFINE(Path, absolute)
STDFS_FUNC_DEFINE(Path, canonical)
STDFS_FUNC_DEFINE(Path, weakly_canonical)

STDFS_FUNC_DEFINE(bool, is_directory)
STDFS_FUNC_DEFINE(bool, is_regular_file)
STDFS_FUNC_DEFINE(bool, is_symlink)
STDFS_FUNC_DEFINE(bool, is_block_file)
STDFS_FUNC_DEFINE(bool, is_character_file)
STDFS_FUNC_DEFINE(bool, is_empty)
STDFS_FUNC_DEFINE(bool, is_fifo)
STDFS_FUNC_DEFINE(bool, is_other)
STDFS_FUNC_DEFINE(bool, is_socket)

inline Path relative (const Path& p, const Path& base = current_path()) { return std::filesystem::relative(p, base); }
inline Path proximate(const Path& p, const Path& base = current_path()) { return std::filesystem::proximate(p, base); }

#undef STDFS_FUNC_DEFINE

class DirectoryEntry
{
public:
	DirectoryEntry() noexcept = default;
	DirectoryEntry(const DirectoryEntry&) = default;
	DirectoryEntry(DirectoryEntry&&) noexcept = default;

	explicit DirectoryEntry(const Path& p) : entry_path{ p } {}
	explicit DirectoryEntry(const QFileInfo& f)
		: entry_path{ f.filePath() }, fs_info{ f } {}

	void refresh() { stat().refresh(); }

	Path path() const { return entry_path; }
	operator const Path&() const { return entry_path; }

	QFileInfo file_info() const { return stat(); }
	operator const QFileInfo&() const { return stat(); }

	bool exists() const { return stat().exists(); }

	bool is_block_file() const { return ::QtFileSystem::is_block_file(entry_path); }
	bool is_character_file() const { return ::QtFileSystem::is_character_file(entry_path); }
	bool is_fifo() const { return ::QtFileSystem::is_fifo(entry_path); }
	bool is_socket() const { return ::QtFileSystem::is_socket(entry_path); }

	bool is_regular_file() const { return stat().isFile(); }
	bool is_directory() const { return stat().isDir(); }
	bool is_symlink() const { return stat().isSymbolicLink(); }
	bool is_other() const { return !is_regular_file() && !is_directory() && !is_symlink(); }

	auto file_size() const { return stat().size(); }
	// hard_link_count

	QDateTime last_write_time() const { return stat().lastModified(); }
	QDateTime last_write_time(const QTimeZone& tz) const { return stat().lastModified(tz); }
	QDateTime last_read_time() const { return stat().lastRead(); }
	QDateTime last_read_time(const QTimeZone& tz) const { return stat().lastRead(tz); }


	DirectoryEntry& operator=(const DirectoryEntry&) = default;
	DirectoryEntry& operator=(DirectoryEntry&&) noexcept = default;

	DirectoryEntry& operator=(const Path& p)
	{
		entry_path = p;
		if (fs_info) fs_info.reset();
		return *this;
	}
private:
	Path entry_path;
	mutable std::optional<QFileInfo> fs_info;

	QFileInfo& stat() const
	{
		if (!fs_info)
			fs_info = QFileInfo{ entry_path };
		return fs_info.value();
	}
public:
	bool operator==(const DirectoryEntry& rhs) const
	{ return entry_path == rhs.entry_path; }
	auto operator<=>(const DirectoryEntry& rhs) const
	{ return entry_path <=> rhs.entry_path; }
};


class DirectoryEntryList
{
public:
	struct const_iterator
	{
		Path parent;
		QStringList::const_iterator it;

		DirectoryEntry operator*() const { return DirectoryEntry{ parent / *it }; }


		const_iterator& operator++()
		{
			++it;
			return *this;
		}

		bool operator==(const const_iterator& other) const
		{ return it == other.it; }
	};

	const_iterator begin() const noexcept { return { path, entries.begin() }; }
	const_iterator end()   const noexcept { return { path, entries.end() }; }

	qsizetype size() const noexcept { return entries.size(); }


	DirectoryEntryList(const Path& path, QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort)
		: path{ path }
		, entries{ QDir{ path }.entryList(filters, sort) }
		, filters{ filters }, sort_flags{ sort } {}

	DirectoryEntryList(const Path& path, const QStringList& name_filter, QDir::Filters filters = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort)
		: path{ path }
		, entries{ QDir{ path }.entryList(name_filter, filters, sort) }
		, filters{ filters }, sort_flags{ sort } {}


	QDir::Filters filter() const noexcept { return filters; }
	QDir::SortFlags sort_flag() const noexcept { return sort_flags; }

private:
	Path path;
	QStringList entries;
	QDir::Filters filters;
	QDir::SortFlags sort_flags;
};


}

using QFileSystemPath = QtFileSystem::Path;
