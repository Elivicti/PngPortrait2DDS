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
	auto role_type = (PortraitTreeWidget::ItemRole)item->data(0, Qt::UserRole).toInt();
	if (role_type == PortraitTreeWidget::ItemRole::DirectoryItem)
		return static_cast<PortraitDirectoryItem*>(item);

	return nullptr;
}
template<>
PortraitItem* cast_item(QTreeWidgetItem* item)
{
	auto role_type = (PortraitTreeWidget::ItemRole)item->data(0, Qt::UserRole).toInt();
	if (role_type == PortraitTreeWidget::ItemRole::PortraitItem)
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

	auto dir_item = new PortraitDirectoryItem{ this, portrait_dir.path };
	dir_item->setText(0, portrait_dir.path.generic_qstring());


	this->addTopLevelItem(dir_item);
	dir_item->setFlags(Qt::ItemIsEnabled | Qt::ItemIsUserCheckable);
	dir_item->setCheckState(0, Qt::Checked);

	for (auto& p : portrait_dir)
	{
		auto item = new PortraitItem{ dir_item, portrait_dir.path / p.filename, p };
		item->setText(0, p.filename);
		item->setCheckState(0, Qt::Checked);

		dir_item->addChild(item);
	}
	dir_item->setExpanded(true);

}



PortraitDirectoryItem::PortraitDirectoryItem(PortraitTreeWidget* parent, const QtFileSystem::Path& path)
	: QTreeWidgetItem{ parent }
	, directory_path{ path }
{
	this->setData(0, Qt::UserRole, (int)PortraitTreeWidget::ItemRole::DirectoryItem);
}


PortraitItem::PortraitItem(PortraitDirectoryItem* parent, const QtFileSystem::Path& path, Portrait& p)
	: QTreeWidgetItem{ parent }
	, portrait_path{ path }
	, portrait{ p }
{
	this->setData(0, Qt::UserRole, (int)PortraitTreeWidget::ItemRole::PortraitItem);
}
