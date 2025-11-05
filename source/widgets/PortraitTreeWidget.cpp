#include "widgets/PortraitTreeWidget.h"

#include "utils/misc.h"

PortraitTreeWidget::PortraitTreeWidget(QWidget* parent)
	: QTreeWidget{ parent }
	, portraits{ new PortraitManager{} }
	, current_portrait_item{ nullptr }
{
	connect(this, &QTreeWidget::itemSelectionChanged, this, &PortraitTreeWidget::on_item_selection_changed);
	connect(this, &QTreeWidget::itemChanged,          this, &PortraitTreeWidget::on_item_changed);

}

PortraitTreeWidget::~PortraitTreeWidget() {}

template<typename T>
static T* cast_item(QTreeWidgetItem* item);

template<>
PortraitDirectoryItem* cast_item(QTreeWidgetItem* item)
{
	auto type = (PortraitTreeWidget::ItemType)item->type();
	if (type == PortraitTreeWidget::ItemType::DirectoryItem)
		return static_cast<PortraitDirectoryItem*>(item);

	return nullptr;
}
template<>
PortraitItem* cast_item(QTreeWidgetItem* item)
{
	auto type = (PortraitTreeWidget::ItemType)item->type();
	if (type == PortraitTreeWidget::ItemType::PortraitItem)
		return static_cast<PortraitItem*>(item);

	return nullptr;
}

void PortraitTreeWidget::on_item_changed(QTreeWidgetItem* item, int column)
{
	SignalBlockerGuard guard{ this };

	Qt::CheckState state = item->checkState(0);
	if (auto dir = cast_item<PortraitDirectoryItem>(item))
	{
		if (state == Qt::PartiallyChecked)
			return;

		int child_cnt = dir->childCount();
		for (int i = 0; i < child_cnt; i++)
		{
			auto child = dir->child(i);
			child->setCheckState(0, state);
		}
		return;
	}
	if (auto portrait = cast_item<PortraitItem>(item))
	{
		// if state ==   checked &&  all_of(children is   checked) -> parent =           checked
		// if state ==   checked && !all_of(children is   checked) -> parent = partially checked
		//
		// if state == unchecked &&  all_of(children is unchecked) -> parent =         unchecked
		// if state == unchecked && !all_of(children is unchecked) -> parent = partially checked
		auto dir = portrait->parent();
		int	child_cnt = dir->childCount();
		bool all_same = true;
		for (int i = 0; i < child_cnt; i++)
		{
			auto child = dir->child(i);
			all_same = all_same && child->checkState(0) == state;

			if (!all_same) break;
		}
		dir->setCheckState(0, all_same ? state : Qt::PartiallyChecked);
		return;
	}

	// UNREACHABLE
	// throw std::runtime_error{ "reached impossible branch" };
}

void PortraitTreeWidget::on_item_selection_changed()
{
	auto item = this->currentItem();
	if (auto dir = cast_item<PortraitDirectoryItem>(item))
	{
		qDebug() << "DIR:" << dir->path();
		return;
	}
	if (auto portrait = cast_item<PortraitItem>(item))
	{
		qDebug() << "Portrait:" << portrait->path();
		current_portrait_item = portrait;
		Q_EMIT portraitSelected(portrait);
		return;
	}

	// UNREACHABLE
	throw std::runtime_error{ "reached impossible branch" };
}

void PortraitTreeWidget::addDirectory(const QtFileSystem::Path& dir)
{
	if (portraits->containsDirectory(dir))
		return;

	PortraitDirectory& portrait_dir = portraits->addDirectory(dir);
	SignalBlockerGuard guard{ this };

	add_directory(portrait_dir);
}

void PortraitTreeWidget::add_directory(PortraitManager::reference dir)
{
	refresh_directory(new PortraitDirectoryItem{ this, dir });
}

void PortraitTreeWidget::refresh_directory(PortraitDirectoryItem* dir)
{
	int last_selected_item_idx = -1; // idx is only meaningful if >= 0

	// need to re-select item after refreshed, save index here
	if (current_portrait_item)
	{
		last_selected_item_idx = dir->indexOfChild(current_portrait_item);
		if (last_selected_item_idx < 0)
			current_portrait_item = nullptr;
	}
	qDeleteAll(dir->takeChildren()); // clear directory item first

	dir->setText(0, dir->path().generic_qstring());

	this->addTopLevelItem(dir);
	dir->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	dir->setCheckState(0, Qt::Checked);

	for (auto& p : dir->data())
	{
		auto item = new PortraitItem{ dir, dir->path() / p.filename, p };
		item->setText(0, p.filename);
		item->setCheckState(0, Qt::Checked);

		// do a count-down to check if this item is previously selected
		// if idx is initially < 0, this condition will never be true
		if ((last_selected_item_idx--) == 0)
			current_portrait_item = item;

		dir->addChild(item);
	}
	dir->setExpanded(true);
}

void PortraitTreeWidget::contextMenuEvent(QContextMenuEvent* evt)
{
	auto item = this->itemAt(evt->pos());
	if (!item)
	{
		evt->ignore();
		return;
	}

	// TODO: fully implement context menu

	if (auto portrait_dir = cast_item<PortraitDirectoryItem>(item))
	{
		ctx_menu.state.dir_item = portrait_dir;

		ctx_menu.directory_item_menu->exec(evt->globalPos());
		return;
	}
	if (auto portrait = cast_item<PortraitItem>(item))
	{
		ctx_menu.state.portrait_item = portrait;

		ctx_menu.portrait_item_menu->exec(evt->globalPos());
		return;
	}

	evt->ignore(); // Should be impossible
}


PortraitDirectoryItem::PortraitDirectoryItem(PortraitTreeWidget* parent, PortraitDirectory& directory)
	: QTreeWidgetItem{ parent, Type }
	, directory{ directory }
{
}

PortraitItem::PortraitItem(PortraitDirectoryItem* parent, const QtFileSystem::Path& full_path, Portrait& p)
	: QTreeWidgetItem{ parent, Type }
	, full_path{ full_path }, portrait{ p }
{
}
