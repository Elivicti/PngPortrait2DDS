#include "PortraitTable.h"
#include <QStandardItemModel>

#include <QHeaderView>
#include <QLayout>
#include <QCheckBox>

#include <array>

PortraitTable::PortraitTable(QWidget *parent)
	: QTableWidget(parent)
{
	setHeaders();

	this->verticalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
	this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeMode::Fixed);

	connect(this, &PortraitTable::tableLoadingCompleted, [this]() {
		setHeaders();
		this->viewport()->update();
	});
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
	this->clear();
	for (auto& row : cbUsingTypes)
		row.clear();
	cbUsingTypes.clear();

	int maxRow = portraits.size();
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
	emit tableLoadingCompleted();
}

void PortraitTable::loadPortraitsInfo(const PresetData& preset)
{
	portraits = preset.portraitData();
	this->clear();
	for (auto& row : cbUsingTypes)
		row.clear();
	cbUsingTypes.clear();

	int maxRow = portraits->size();
	this->setRowCount(maxRow);
	cbUsingTypes.fill(QList<QCheckBox*>{}, maxRow);

	for (int row = 0; row < maxRow; row++)
	{
		auto usage{ preset.usage(portraits->at(row).fileName()) };
		int use_types[3] = {
			usage.species, usage.leaders, usage.rulers
		};
		this->setItem(row, 0, new QTableWidgetItem(portraits->at(row).fileName()));
		for (int i = 1; i < 4; i++)
		{
			QWidget* widget = new QWidget(this);
			QGridLayout* layout = new QGridLayout(widget);
			QCheckBox* box = new QCheckBox(widget);
			cbUsingTypes[row].append(box);

			box->setObjectName(QString::fromUtf8("cbIsUsingThisType"));
			box->setChecked(use_types[i - 1]);
			layout->addWidget(box);
			layout->setAlignment(Qt::AlignCenter);
			widget->setLayout(layout);
			this->setCellWidget(row, i, widget);
		}
	}
	emit tableLoadingCompleted();

}

void PortraitTable::appendPortraitInfo(const QString& pic, bool species, bool leader, bool ruler)
{
	int row = this->rowCount();
	this->setRowCount(row + 1);

	cbUsingTypes.append(QList<QCheckBox*>());
	
	std::array<int, 3> state = { species, leader, ruler };

	this->setItem(row, 0, new QTableWidgetItem(pic));
	for (int i = 1; i < 4; i++)
	{
		QWidget* widget = new QWidget(this);
		QGridLayout* layout = new QGridLayout(widget);
		QCheckBox* box = new QCheckBox(widget);
		cbUsingTypes[row].append(box);

		box->setObjectName(QString::fromUtf8("cbIsUsingThisType"));
		box->setChecked(state[i - 1]);
		layout->addWidget(box);
		layout->setAlignment(Qt::AlignCenter);
		widget->setLayout(layout);
		this->setCellWidget(row, i, widget);
	}
}

bool PortraitTable::isUsingPortraitType(int row, PortraitUsingType t)
{
	return cbUsingTypes[row][(int)t]->isChecked();
}

void PortraitTable::clearContents()
{
	for (auto& row : cbUsingTypes)
	{
		for (auto cb : row)
			delete cb;
		row.clear();
	}
	cbUsingTypes.clear();
	QTableWidget::clearContents();
	this->setRowCount(0);
}