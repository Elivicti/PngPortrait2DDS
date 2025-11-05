#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "utils/QtFileSystem.h"
#include "PortraitData.h"

class PortraitDirectoryItem;
class PortraitItem;

class PortraitTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	enum class ItemType : std::uint16_t
	{
		DirectoryItem = QTreeWidgetItem::UserType + 2,
		PortraitItem  = QTreeWidgetItem::UserType + 3
	};
public:
	PortraitTreeWidget(QWidget* parent = nullptr);
	~PortraitTreeWidget();

	void loadFromPreset(const QtFileSystem::Path& preset);
	void addDirectory(const QtFileSystem::Path& dir);

	FlatPortraitView portraitsView() { return { *portraits }; }

	PortraitItem* currentPortraitItem() { return current_portrait_item; }

Q_SIGNALS:
	void portraitSelected(PortraitItem* item);

private Q_SLOTS:
	void on_item_selection_changed();
	void on_item_changed(QTreeWidgetItem* item, int column);


private:
	void add_directory(PortraitManager::reference dir);
	void refresh_directory(PortraitDirectoryItem* dir);

private:
	std::unique_ptr<PortraitManager> portraits;

	PortraitItem* current_portrait_item;

};

// Item to represent directory
class PortraitDirectoryItem : public QTreeWidgetItem
{
public:
	enum { Type = (int)PortraitTreeWidget::ItemType::DirectoryItem };
	PortraitDirectoryItem(PortraitTreeWidget* parent, PortraitDirectory& directory);

	const QtFileSystem::Path& path() const { return directory.path; }

	PortraitDirectory& data() { return directory; }

private:
	PortraitDirectory& directory;
};


// Item to represent portrait
class PortraitItem : public QTreeWidgetItem
{
public:
	enum { Type = (int)PortraitTreeWidget::ItemType::PortraitItem };
	PortraitItem(PortraitDirectoryItem* parent, const QtFileSystem::Path& full_path, Portrait& p);

	const QtFileSystem::Path& fullPath() const { return full_path; }

	Portrait& data() { return portrait; }

private:
	QtFileSystem::Path full_path;
	Portrait& portrait;
};
