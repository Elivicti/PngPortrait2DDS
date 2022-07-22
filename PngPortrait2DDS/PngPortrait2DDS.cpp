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
	, ui(new Ui::PngPortrait2DDSClass()), lbLog(new QLabel(this)), dataLoaded(false)
	, data(0, 0, 0.20), currIndex(0)
	, nvdxt(new QProcess(this))
{
	ui->setupUi(this);
	ui->statusBar->addPermanentWidget(lbLog);

	this->disableEditorWidget();

	ui->wPreview->scalePng(0.20);

	connect(ui->btnBrowseDirectory, &QPushButton::clicked, this, &PngPortrait2DDS::onBrowseDirectoryClicked);
	connect(ui->tbvPngItems, &QAbstractItemView::activated, this, &PngPortrait2DDS::onPortraitSelected);

	connect(ui->cbUseSeperateSetting, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onUseSeperateSettingStateChanged);

	connect(ui->dspbPotraitScale, &QDoubleSpinBox::valueChanged, [this](double value) {
		data.setScale(currIndex, value);
		ui->hsldPotraitScale->setValue(value * ScaleSliderMaxValue);
		ui->wPreview->scalePng(value);
		ui->wPreview->repaint();
	});
	connect(ui->hsldPotraitScale, &QSlider::sliderMoved, [this](int value) {
		double scale = (double)value / (double)ScaleSliderMaxValue;
		data.setScale(currIndex, scale);
		ui->dspbPotraitScale->setValue(scale);
		ui->wPreview->scalePng(scale);
		ui->wPreview->repaint();
	});
	connect(ui->wPreview, &PreviewWidget::imageScaleChangedByWheel, [this](double ratio) {
		ui->dspbPotraitScale->setValue(ratio);
		ui->hsldPotraitScale->setValue(ratio * ScaleSliderMaxValue);
	});

	connect(ui->wPreview, &PreviewWidget::imageOffsetChangedByCursor, [this](const QPoint& p) {
		ui->spbPortraitOffsetX->setValue(p.x());
		ui->spbPortraitOffsetY->setValue(p.y());
		data.setOffset(currIndex, p);
	});
	connect(ui->spbPortraitOffsetX, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageOffsetXChanged);
	connect(ui->spbPortraitOffsetY, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageOffsetYChanged);


	QSize size = ui->wPreview->size();
	ui->spbImageWidth->setValue(size.width());
	ui->spbImageHeight->setValue(size.height());
	connect(ui->spbImageHeight, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageSizeChanged);
	connect(ui->spbImageWidth, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageSizeChanged);

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
	QString path = QFileDialog::getExistingDirectory(this, tr("Select Portrait Directory"), "./");
	if (!path.isEmpty())
	{
		ui->statusBar->showMessage(tr("Loading..."));

		data.clear();
		ui->lePngDirectory->setText(path);
		QDir dir(path);
		QStringList filter(QString("*.png"));
		QFileInfoList pngSources = dir.entryInfoList(filter);
		for (auto& png : pngSources)
			data.append(new PortraitData(png));

		ui->tbvPngItems->setPortraitsInfo(dir.entryList(filter));

		ui->statusBar->showMessage(tr("Load Completed: %1 Total").arg(pngSources.size()));

		this->enableEditorWidget();
		dataLoaded = true;
	}
}

void PngPortrait2DDS::onPortraitSelected(const QModelIndex& index)
{
	qDebug() << data.at(-1)->offset;
	qDebug() << data.at(-1)->scale << "\n";

	if (index.isValid())
	{
		currIndex = index.row();

		qDebug() << currIndex;
		qDebug() << data.portrait(currIndex);
		qDebug() << data.at(currIndex)->useSeperateSetting << " " << (currIndex) * data.at(currIndex)->useSeperateSetting;
		qDebug() << data.offset(currIndex);
		qDebug() << data.scale(currIndex) << "\n";

		ui->wPreview->setPreviewPng(data.portrait(currIndex).absoluteFilePath());

		QPoint& offset = data.offset(currIndex);
		double scale = data.scale(currIndex);

		ui->spbPortraitOffsetX->setValue(offset.x());
		ui->spbPortraitOffsetY->setValue(offset.y());

		ui->dspbPotraitScale->setValue(scale);
		ui->hsldPotraitScale->setValue(scale * ScaleSliderMaxValue);

		ui->cbUseSeperateSetting->setChecked(data.at(currIndex)->useSeperateSetting);
		
	}
}

void PngPortrait2DDS::onUseSeperateSettingStateChanged(int state)
{
	data.at(currIndex)->useSeperateSetting = (state == Qt::Checked);
	if (data.at(currIndex)->useSeperateSetting)
	{
		data.at(currIndex)->offset = ui->wPreview->getImageOffset();
		data.at(currIndex)->scale = ui->wPreview->getScaleRatio();
	}
	else
	{
		QPoint offset(data.offset(currIndex));
		double scale = data.scale(currIndex);
		ui->spbPortraitOffsetX->setValue(offset.x());
		ui->spbPortraitOffsetY->setValue(offset.y());
		ui->dspbPotraitScale->setValue(scale);
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

void PngPortrait2DDS::onImageOffsetXChanged(int value)
{
	if (!ui->wPreview->isCursorMovingImage())
	{
		int x = ui->spbPortraitOffsetX->value();
		ui->wPreview->setImageOffsetX(x);
		data.setOffsetX(currIndex , x);

		ui->wPreview->repaint();
	}
}
void PngPortrait2DDS::onImageOffsetYChanged(int value)
{
	if (!ui->wPreview->isCursorMovingImage())
	{
		int y =  ui->spbPortraitOffsetY->value();
		ui->wPreview->setImageOffsetY(y);
		data.setOffsetY(currIndex, y);

		ui->wPreview->repaint();
	}
}

void PngPortrait2DDS::disableEditorWidget()
{
	ui->spbImageHeight->setEnabled(false);
	ui->spbImageWidth->setEnabled(false);
	ui->cbUseSeperateSetting->setEnabled(false);
	ui->spbPortraitOffsetX->setEnabled(false);
	ui->spbPortraitOffsetY->setEnabled(false);
	ui->dspbPotraitScale->setEnabled(false);
	ui->hsldPotraitScale->setEnabled(false);
	ui->btnExport->setEnabled(false);
}
void PngPortrait2DDS::enableEditorWidget()
{
	ui->spbImageHeight->setEnabled(true);
	ui->spbImageWidth->setEnabled(true);
	ui->cbUseSeperateSetting->setEnabled(true);
	ui->spbPortraitOffsetX->setEnabled(true);
	ui->spbPortraitOffsetY->setEnabled(true);
	ui->dspbPotraitScale->setEnabled(true);
	ui->hsldPotraitScale->setEnabled(true);
	ui->btnExport->setEnabled(dataLoaded);
}

void PngPortrait2DDS::onExport()
{
	// initialize base image
	QPixmap img(ui->wPreview->size());
	img.fill(QColor(0, 0, 0, 0));

	// load portrait
	QPixmap portrait(data.at(currIndex)->portrait.absoluteFilePath());
	
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