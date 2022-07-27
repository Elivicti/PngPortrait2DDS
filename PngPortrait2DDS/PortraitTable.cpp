#include "PortraitTable.h"
#include <QStandardItemModel>

#include <QHeaderView>
#include <QLayout>
#include <QCheckBox>

#include <QProgressDialog>
#include <QtConcurrent>


PortraitTable::PortraitTable(QWidget *parent)
	: QTableWidget(parent)
{
	setHeaders();

	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

	
}

PortraitTable::~PortraitTable()
{}

void PortraitTable::resizeEvent(QResizeEvent* evt)
{
	resizeHeaders();
}

void PortraitTable::setHeaders()
{
	this->setColumnCount(4);
	QStringList headers({ tr("Portrait"), tr("Species"), tr("leader"), tr("Ruler") });
	this->setHorizontalHeaderLabels(headers);
}

void PortraitTable::resizeHeaders()
{
	QHeaderView* header = this->horizontalHeader();
	header->resizeSection(0, this->size().width() - 3 * 54 - 18);
	header->resizeSection(1, 54);
	header->resizeSection(2, 54);
	header->resizeSection(3, 54);
}

void PortraitTable::setPortraitsInfo(const QStringList& portraits)
{
	int maxRow = portraits.size();

	this->clear();
	for (auto& row : cbUsingTypes)
		row.clear();
	cbUsingTypes.clear();

	this->setRowCount(maxRow);

	for (int row = 0; row < maxRow; row++)
	{

		cbUsingTypes.append(QList<QCheckBox*>());

		this->setItem(row, 0, new QTableWidgetItem(portraits.at(row)));
		for (int i = 1; i < 4; i++)
		{
			QWidget* widget = new QWidget(this);
			QGridLayout* layout = new QGridLayout(widget);
			QCheckBox* box = new QCheckBox(widget);
			cbUsingTypes[row].append(box);

			box->setObjectName(QString::fromUtf8("cbIsUsingThisType"));
			box->setChecked(true);
			layout->addWidget(box);
			layout->setAlignment(Qt::AlignCenter);
			widget->setLayout(layout);
			this->setCellWidget(row, i, widget);
		}
	}
	

	setHeaders();
	this->viewport()->update();
	emit tableLoadingCompleted();

}

bool PortraitTable::isUsingPortraitType(int row, PortraitUsingType t)
{
	return cbUsingTypes[row][(int)t]->isChecked();
}