#include "PngPortrait2DDS.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QDir>
#include <QPainter>

#include <QDebug>

#define ScaleSliderMaxValue 1000

PngPortrait2DDS::PngPortrait2DDS(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::PngPortrait2DDSClass()), lbLog(new QLabel(this))
	, nvdxt(new QProcess(this))
{
	ui->setupUi(this);
	ui->statusBar->addPermanentWidget(lbLog);

	ui->wPreview->scalePng(0.22);
	connect(ui->btnBrowseDirectory, &QPushButton::clicked, this, &PngPortrait2DDS::onBrowseDirectoryClicked);
	connect(ui->tbvPngItems, &QAbstractItemView::activated, [this](const QModelIndex& index) {
		if (index.isValid())
		{
			ui->wPreview->setPreviewPng(pngSources[index.row()].absoluteFilePath());
			ui->dspbPotraitScale->setValue(ui->wPreview->getScaleRatio());
			ui->hsldPotraitScale->setValue(ui->wPreview->getScaleRatio() * ScaleSliderMaxValue);
		}
	});

	connect(ui->dspbPotraitScale, &QDoubleSpinBox::valueChanged, [this](double value) {
		ui->hsldPotraitScale->setValue(value * 1000);
		ui->wPreview->scalePng(value);
		ui->wPreview->repaint();
	});
	connect(ui->hsldPotraitScale, &QSlider::sliderMoved, [this](int value) {
		ui->dspbPotraitScale->setValue((double)value / (double)ScaleSliderMaxValue);
		ui->wPreview->scalePng((double)value / (double)ScaleSliderMaxValue);
		ui->wPreview->repaint();
	});

	connect(ui->wPreview, &PreviewWidget::imageScaleChangedByWheel, [this](double ratio) {
		ui->dspbPotraitScale->setValue(ratio);
		ui->hsldPotraitScale->setValue(ratio * ScaleSliderMaxValue);
	});

	QSize size = ui->wPreview->size();
	ui->spbImageWidth->setValue(size.width());
	ui->spbImageHeight->setValue(size.height());

	connect(ui->spbImageHeight, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageSizeChanged);
	connect(ui->spbImageWidth,  &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageSizeChanged);

	connect(ui->wPreview, &PreviewWidget::imageOffsetChangedByCursor, [this](const QPoint& p) {
		ui->spbPortraitOffsetX->setValue(p.x());
		ui->spbPortraitOffsetY->setValue(p.y());
	});

	connect(ui->spbPortraitOffsetX, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageOffsetChanged);
	connect(ui->spbPortraitOffsetY, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageOffsetChanged);



	connect(ui->btnExport, &QPushButton::clicked, this, &PngPortrait2DDS::onExport);
}

PngPortrait2DDS::~PngPortrait2DDS()
{
	delete ui;
}

void PngPortrait2DDS::show()
{
	QMainWindow::show();
	this->onImageSizeChanged(0);
}

void PngPortrait2DDS::onBrowseDirectoryClicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Select Image", "./");
	if (!dir.isEmpty())
	{
		ui->lePngDirectory->setText(dir);
		pngSources = QDir(dir).entryInfoList(QStringList(QString("*.png")));
		ui->tbvPngItems->setPortraitsInfo(&pngSources);

		ui->statusBar->showMessage(tr("Load Completed: %1 Total").arg(pngSources.size()));
		
	}
}

void PngPortrait2DDS::onImageSizeChanged(int value)
{
	int height = ui->spbImageHeight->value();
	int width = ui->spbImageWidth->value();
	QSize frameSize = ui->fmPreview->size();
	QSize imgSize(width, height);
	int margin = 30;

	if (height >= frameSize.height() - margin || width >= frameSize.width() - margin)
	{

		double ratio = (double)height / (double)width;
		// make sure scaled height(new height) is smaller than frame height
		if (height >= width || (frameSize.width() - margin) * ratio >= frameSize.height() - margin)
		{
			height = frameSize.height() - margin;
			width = height / ratio;
		}
		else
		{
			width = frameSize.width() - margin;
			height = width * ratio;
		}
	}

	ui->wPreview->move((frameSize.width() - width) / 2, (frameSize.height() - height) / 2);
	ui->wPreview->scaledResize(width, height, imgSize);

	ui->fmPreview->repaint();
}

void PngPortrait2DDS::onImageOffsetChanged(int value)
{
	if (!ui->wPreview->isCursorMovingImage())
	{
		int x = ui->spbPortraitOffsetX->value();
		int y = ui->spbPortraitOffsetY->value();
		ui->wPreview->setImageOffset(x, y);
		ui->wPreview->repaint();
	}
}

void PngPortrait2DDS::onExport()
{
	// initialize base image
	QPixmap img(ui->wPreview->size());
	img.fill(QColor(0, 0, 0, 0));

	// load portrait
	QPixmap portrait(pngSources[ui->tbvPngItems->currentIndex().row()].absoluteFilePath());
	
	// read preset
	QPoint offset = ui->wPreview->getImageOffset();
	double scale = ui->wPreview->getScaleRatio();

	// draw & save
	QPainter painter(&img);
	painter.drawPixmap(offset, portrait.scaled(portrait.size() * scale, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation));
	img.save("test.png");
	

	// png2dds command line
	// nvdxt.exe -file input.png -alpha [-scale 2] -output output.dds
	// to dds
	nvdxt->start(QString("./nvdxt.exe"), { "-file", "test.png", "-alpha", "-output", "output.dds" });
}

void PngPortrait2DDS::resizeEvent(QResizeEvent* evt)
{
	this->onImageSizeChanged(0);
}