#include "PngPortrait2DDS.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QStringListModel>
#include <QDir>



#include <QDebug>

PngPortrait2DDS::PngPortrait2DDS(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::PngPortrait2DDSClass())
{
	ui->setupUi(this);
	ui->wPreview->scalePng(0.22);
	connect(ui->btnBrowsDirectory, SIGNAL(clicked()), this, SLOT(onBrowsDirectoryClicked()));
	connect(ui->lvPngItems, &QAbstractItemView::doubleClicked, [this](const QModelIndex& index) {
		if (index.isValid())
		{
			ui->wPreview->setPreviewPng(pngSources[index.row()].absoluteFilePath());
			ui->dspbImageScale->setValue(ui->wPreview->getScaleRatio());
			ui->hsldImageScale->setValue(ui->wPreview->getScaleRatio() * 100);
		}
	});

	connect(ui->dspbImageScale, &QDoubleSpinBox::valueChanged, [this](double value) {
		ui->hsldImageScale->setValue(value * 100);
		ui->wPreview->scalePng(value);
		ui->wPreview->repaint();
	});

	connect(ui->hsldImageScale, &QSlider::sliderMoved, [this](int value) {
		ui->dspbImageScale->setValue((double)value / 100.0);
		ui->wPreview->scalePng((double)value / 100.0);
		ui->wPreview->repaint();
	});
}

PngPortrait2DDS::~PngPortrait2DDS()
{
	delete ui;
}


void PngPortrait2DDS::onBrowsDirectoryClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select Image", "./");
	if (!dir.isEmpty())
	{
		ui->lePngDirectory->setText(dir);
		pngSources = QDir(dir).entryInfoList(QStringList(QString("*.png")));

		QStringList fileNames;
		for (auto& png : pngSources)
			fileNames.append(png.fileName());
		QStringListModel* model = new QStringListModel(fileNames, this);
		ui->lvPngItems->setModel(model);
	}
}