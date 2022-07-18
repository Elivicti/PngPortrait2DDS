#include "PortraitTableView.h"
#include <QStandardItemModel>

#include <QHeaderView>
#include <QLayout>
#include <QCheckBox>




PortraitTableView::PortraitTableView(QWidget *parent)
	: QTableView(parent)
	, itemModel(new QStandardItemModel(this))
{
	this->setModel(itemModel);

	setHeaders();

	
	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);
}

PortraitTableView::~PortraitTableView()
{}

void PortraitTableView::resizeEvent(QResizeEvent* evt)
{
	resizeHeaders();
}

void PortraitTableView::setHeaders()
{
	QStringList headers({ tr("Portrait"), tr("Species"), tr("leader"), tr("Ruler") });
	itemModel->setHorizontalHeaderLabels(headers);
}

void PortraitTableView::resizeHeaders()
{
	QHeaderView* header = this->horizontalHeader();
	header->resizeSection(0, this->size().width() - 3 * 54 - 18);
	header->resizeSection(1, 54);
	header->resizeSection(2, 54);
	header->resizeSection(3, 54);
}

void PortraitTableView::setPortraitsInfo(QFileInfoList* portraits)
{
	itemModel->clear();

	int maxRow = portraits->size();
	for (int row = 0; row < maxRow; row++)
	{
		QStandardItem* filename = new QStandardItem(portraits->at(row).fileName());
		itemModel->appendRow({ filename, new QStandardItem(), new QStandardItem(), new QStandardItem()});
		for (int i = 1; i < 4; i++)
		{
			QWidget* widget = new QWidget(this);
			QGridLayout* layout = new QGridLayout(widget);
			QCheckBox* box = new QCheckBox(widget);
			box->setChecked(true);
			layout->addWidget(box);
			layout->setAlignment(Qt::AlignCenter);
			widget->setLayout(layout);
			this->setIndexWidget(itemModel->index(row, i), widget);
		}
	}

	setHeaders();
	this->viewport()->update();
}