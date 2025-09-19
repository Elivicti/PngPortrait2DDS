#pragma once

#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "utils/QtFileSystem.h"

class PortraitManager;

class PortraitDirectoryItem;
class PortraitItem;

class PortraitTreeWidget : public QTreeWidget
{
	Q_OBJECT
public:
	enum class ItemRole : std::uint16_t
	{
		DirectoryItem = 2,
		PortraitItem  = 3
	};
public:
	PortraitTreeWidget(QWidget* parent = nullptr);
	~PortraitTreeWidget();

	void loadFromPreset(const QtFileSystem::Path& preset);
	void addDirectory(const QtFileSystem::Path& dir);

Q_SIGNALS:
	void portraitSelected(PortraitItem* item);

private Q_SLOTS:
	void on_item_selection_changed();
	void on_item_changed(QTreeWidgetItem* item, int column);

private:
	std::unique_ptr<PortraitManager> portraits;

};

class PortraitDirectoryItem : public QTreeWidgetItem
{
public:
	PortraitDirectoryItem(PortraitTreeWidget* parent, const QtFileSystem::Path& path);


	const QtFileSystem::Path& path() const { return directory_path; }

private:
	QtFileSystem::Path directory_path;

};


class PortraitItem : public QTreeWidgetItem
{
public:
	PortraitItem(PortraitDirectoryItem* parent, const QtFileSystem::Path& path);

	const QtFileSystem::Path& path() const { return portrait_path; }

private:
	QtFileSystem::Path portrait_path;

};
