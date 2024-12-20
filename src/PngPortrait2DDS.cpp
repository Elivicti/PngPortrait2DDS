#include "PngPortrait2DDS.h"
#include "ui/ui_PngPortrait2DDS.h"
#include "GlobalConfigManager.h"

#include <QProgressBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPainter>
#include <QMessageBox>
#include <QtConcurrent>

#include <QDropEvent>
#include <QMimeData>

#include <SOIL2/SOIL2.h>

#include <QDebug>

#define ScaleSliderMaxValue 1000
#define DefaultScale (gConfig->scale())

PngPortrait2DDS::PngPortrait2DDS(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::PngPortrait2DDSClass()), lbLog(new QLabel(this)), dataLoaded(false)
	, data{gConfig->imageSize(), gConfig->offset(), DefaultScale }
	, currIndex(0), pgbar(new QProgressBar)
	, python{ this }
{
	// init ui
	ui->setupUi(this);
	ui->statusBar->addPermanentWidget(lbLog);
	ui->spbImageWidth->setValue(data.imageSize().width());
	ui->spbImageHeight->setValue(data.imageSize().height());
	ui->hsldPotraitScale->setValue(data->defaultScale() * ScaleSliderMaxValue);
	ui->dspbPotraitScale->setValue(data->defaultScale());
	ui->cbExportDDS->setChecked(data.exportDDS());
	ui->cbExportRegistration->setChecked(data.exportRegistration());
	ui->cbExportExtraEffect->setChecked(data.exportExtraEffect());

	python.setUpScriptMenu(ui->menuScripts);

	connect(ui->btnBrowseDirectory, &QPushButton::clicked, this, &PngPortrait2DDS::onBrowseDirectoryClicked);

	connect(ui->tbwPngItems, &PortraitTable::tableLoadingCompleted, this, &PngPortrait2DDS::onTableLoadingCompleted);
	connect(ui->tbwPngItems, &QAbstractItemView::activated, this, &PngPortrait2DDS::onPortraitSelected);

	connect(ui->cbUseSeperateSetting, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onUseSeperateSettingStateChanged);

	connect(ui->dspbPotraitScale, &QDoubleSpinBox::valueChanged, [this](double value) {
		if (data->at(currIndex).independent_setting)
			data->setScale(currIndex, value);
		else
			data->setDefaultScale(value);

		ui->hsldPotraitScale->setValue(value * ScaleSliderMaxValue);
		ui->wPreview->scalePng(value);
		ui->wPreview->repaint();
	});
	connect(ui->hsldPotraitScale, &QSlider::sliderMoved, [this](int value) {
		double scale = (double)value / (double)ScaleSliderMaxValue;
		data->setScale(currIndex, scale);
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
		if (data->useIndependentSettings(currIndex))
			data->setOffset(currIndex, p);
		else
			data->setDefaultOffset(p);
		
	});
	connect(ui->spbPortraitOffsetX, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageOffsetXChanged);
	connect(ui->spbPortraitOffsetY, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageOffsetYChanged);


	QSize size = ui->wPreview->size();
	ui->spbImageWidth->setValue(size.width());
	ui->spbImageHeight->setValue(size.height());
	connect(ui->spbImageHeight, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageSizeChanged);
	connect(ui->spbImageWidth, &QSpinBox::valueChanged, this, &PngPortrait2DDS::onImageSizeChanged);

	connect(ui->cbExportDDS, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onExportOptionChanged);
	connect(ui->cbExportRegistration, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onExportOptionChanged);
	connect(ui->cbExportExtraEffect, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onExportOptionChanged);

	connect(ui->btnExport, &QPushButton::clicked, this, &PngPortrait2DDS::onExport);

	connect(ui->actionSavePreset, &QAction::triggered, this, &PngPortrait2DDS::savePresetAsJson);
	connect(ui->actionLoadPreset, &QAction::triggered, [this]() {
		QString filename(QFileDialog::getOpenFileName(this, tr("Select Preset"), "./", tr("Json File(*.json)")));
		if (!filename.isEmpty())
			this->loadPresetFromJson(filename);
	});

	this->setAcceptDrops(true);
	this->disableEditorWidget();

	pgbar->hide();
	ui->statusBar->addPermanentWidget(pgbar);
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
		ui->lePngDirectory->setText(path.replace("/", "\\"));
		PresetLoader loader;
		data = loader.loadFromDirectory(path);
		data.pyScript() = python.enabledScript();
		data.exportDDS() = ui->cbExportDDS->isChecked();
		data.exportRegistration() = ui->cbExportRegistration->isChecked();
		data.exportExtraEffect() = ui->cbExportExtraEffect->isChecked();
		ui->tbwPngItems->loadPortraitsInfo(data);
	}
}

void PngPortrait2DDS::onTableLoadingCompleted()
{
	ui->statusBar->showMessage(tr("Loading Completed: %1 Total").arg(data->size()), 30000);

	pgbar->hide();
	pgbar->reset();
	dataLoaded = true;
	this->enableEditorWidget();
	ui->wPreview->scalePng(data->defaultScale());
	ui->dspbPotraitScale->setValue(data->defaultScale());
	ui->spbPortraitOffsetX->setValue(data->defaultOffset().x());
	ui->spbPortraitOffsetY->setValue(data->defaultOffset().y());

	ui->cbExportDDS->setChecked(data.exportDDS());
	ui->cbExportRegistration->setChecked(data.exportRegistration());
	ui->cbExportExtraEffect->setChecked(data.exportExtraEffect());
}

void PngPortrait2DDS::onPortraitSelected(const QModelIndex& index)
{

	if (index.isValid())
	{
		currIndex = index.row();

		ui->wPreview->setPreviewPng(data->at(currIndex).absoluteFilePath());

		const QPoint& offset = data->offset(currIndex);
		double scale = data->scale(currIndex);

		ui->spbPortraitOffsetX->setValue(offset.x());
		ui->spbPortraitOffsetY->setValue(offset.y());

		ui->dspbPotraitScale->setValue(scale);
		ui->hsldPotraitScale->setValue(scale * ScaleSliderMaxValue);

		ui->cbUseSeperateSetting->setChecked(data->at(currIndex).independent_setting);
	}
}

void PngPortrait2DDS::onUseSeperateSettingStateChanged(int state)
{
	data->setUseIndependentSettings(currIndex, state == Qt::Checked);
	if (data->useIndependentSettings(currIndex))
	{
		data->setOffset(currIndex, ui->wPreview->getImageOffset());
		data->setScale(currIndex, ui->wPreview->getScaleRatio());
	}
	else
	{
		QPoint offset{ data->defaultOffset() };
		double scale { data->defaultScale()  };
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
		if (data->useIndependentSettings(currIndex))
			data->setOffsetX(currIndex, x);
		else
			data->setDefaultOffsetX(x);

		ui->wPreview->repaint();
	}
}
void PngPortrait2DDS::onImageOffsetYChanged(int value)
{
	if (!ui->wPreview->isCursorMovingImage())
	{
		int y = ui->spbPortraitOffsetY->value();
		ui->wPreview->setImageOffsetY(y);
		if (data->useIndependentSettings(currIndex))
			data->setOffsetY(currIndex, y);
		else
			data->setDefaultOffsetY(y);

		ui->wPreview->repaint();
	}
}

void PngPortrait2DDS::disableEditorWidget(bool extra)
{
	ui->spbImageHeight->setEnabled(false);
	ui->spbImageWidth->setEnabled(false);
	ui->cbUseSeperateSetting->setEnabled(false);
	ui->spbPortraitOffsetX->setEnabled(false);
	ui->spbPortraitOffsetY->setEnabled(false);
	ui->dspbPotraitScale->setEnabled(false);
	ui->hsldPotraitScale->setEnabled(false);
	ui->btnExport->setEnabled(false);

	if (extra)
	{
		ui->lePngDirectory->setDisabled(true);
		ui->btnBrowseDirectory->setDisabled(true);
		ui->tbwPngItems->setDisabled(true);
		ui->wPreview->setDisabled(true);
		ui->cbExportDDS->setDisabled(true);
		ui->cbExportRegistration->setDisabled(true);
		ui->cbExportExtraEffect->setDisabled(true);
		ui->menuOptions->setDisabled(true);
	}
}
void PngPortrait2DDS::enableEditorWidget(bool all)
{
	ui->spbImageHeight->setEnabled(true);
	ui->spbImageWidth->setEnabled(true);
	ui->cbUseSeperateSetting->setEnabled(true);
	ui->spbPortraitOffsetX->setEnabled(true);
	ui->spbPortraitOffsetY->setEnabled(true);
	ui->dspbPotraitScale->setEnabled(true);
	ui->hsldPotraitScale->setEnabled(true);
	onExportOptionChanged(0);

	if (all)
	{
		ui->lePngDirectory->setEnabled(true);
		ui->btnBrowseDirectory->setEnabled(true);
		ui->tbwPngItems->setEnabled(true);
		ui->wPreview->setEnabled(true);
		ui->cbExportDDS->setEnabled(true);
		ui->cbExportRegistration->setEnabled(true);
		ui->cbExportExtraEffect->setEnabled(true);
		ui->menuOptions->setEnabled(true);
	}
}

void PngPortrait2DDS::onExportOptionChanged(int state)
{
	bool anyOptionEnable = ui->cbExportDDS->isChecked() 
						|| ui->cbExportRegistration->isChecked() 
						|| ui->cbExportExtraEffect->isChecked();
	ui->btnExport->setEnabled(anyOptionEnable && dataLoaded);
}

void PngPortrait2DDS::onExport()
{

	QSize imgSize(ui->spbImageWidth->value(), ui->spbImageHeight->value());
	if (imgSize.width() % 4 != 0 || imgSize.height() % 4 != 0)
	{
		QMessageBox::critical(
			this,
			tr("Error"),
			tr("The width and height of the image must be a multiple of 4."),
			tr("OK")
		);
		return;
	}

	qsizetype portraitCnt = data->size();
	bool export_dds = ui->cbExportDDS->isChecked();
	bool export_registration = ui->cbExportRegistration->isChecked();
	bool export_effect = ui->cbExportExtraEffect->isChecked();

	// create file structure
	QDir output(data.directory());
	QString path_output = output.dirName() + QString("_output");
	QString name = output.dirName();
	output.cdUp();
	output.mkdir(path_output);
	output.cd(path_output);

	// !! This string must be created after cd
	QString dds_file = output.absoluteFilePath(name + "/%1.dds");
	qDebug() << output.absolutePath();

	QFile registration;
	if (export_registration)
	{
		registration.setFileName(output.absoluteFilePath(name + ".txt"));
		registration.open(QIODevice::ReadWrite | QIODevice::Text | QIODeviceBase::Truncate);
	}

	// In the end, file structure will be something like this:
	// path/							// where png files are stored
	// path_output/						// where all generated files are
	//  |-path/							// generated dds goes here    (if enabled)
	//	|-registration.txt				// portrait registration file (if enabled)
	//	\-....							// other files (may be created during extra effect in python script)


	this->disableEditorWidget(true);
	
	if (export_registration || export_effect)
	{
		bool success = python.runScript(
			data, name, export_registration, export_effect
			, [&registration](const std::vector<std::string>& lines) {
				for (auto& line : lines)
					registration.write(QByteArray::fromStdString(line) + "\n");
			}
			, output.absolutePath());

		if (!success)
		{
			QMessageBox::critical(this, tr("Error"), python.errorMessage(), tr("OK"));
			this->enableEditorWidget(true);
			return;
		}
	}

	if (export_dds)
	{
		PresetData::SharedDataPointer portrait_list = data.portraitData();
		output.mkdir(name);

		QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
		pgbar->show();
		pgbar->setMaximum(portrait_list->size());

		lbLog->setText(tr("Processing images..."));
		connect(watcher, &QFutureWatcher<void>::progressValueChanged, pgbar, &QProgressBar::setValue);
		connect(watcher, &QFutureWatcher<void>::finished, [this, watcher]() {
			lbLog->setText(tr("Completed."));

			pgbar->hide();
			watcher->deleteLater();
			this->enableEditorWidget(true);
		});

		auto png2dds = [this, dds_file, imgSize](const PortraitsData::Portrait& i) {
			const QFileInfo& file = i.portrait;
			std::string ddspath = dds_file.arg(file.baseName()).toStdString();

			// load portrait
			QImage portrait(file.absoluteFilePath());

			// read preset
			QPoint offset{
				i.independent_setting ? i.offset : data->defaultOffset()
			};
			double scale {
				i.independent_setting ? i.scale : data->defaultScale()
			};

			// draw & save
			QImage img(imgSize, QImage::Format_RGBA8888);
			img.fill(QColor(0, 0, 0, 0));
			QPainter painter(&img);

			painter.drawImage(
				offset, portrait.scaled(
					portrait.size() * scale,
					Qt::AspectRatioMode::KeepAspectRatio,
					Qt::TransformationMode::SmoothTransformation
				)
			);

			// save as dxt5
			SOIL_save_image_quality(
				ddspath.c_str(), SOIL_SAVE_TYPE_DDS,
				img.width(), img.height(),
				4,
				img.bits(),
				0
			);
		};

		QFuture<void> future = QtConcurrent::map(data->begin(), data->end(), png2dds);
		watcher->setFuture(future);
	}
	else
	{
		lbLog->setText(tr("Completed."));
		this->enableEditorWidget(true);
	}
}

void PngPortrait2DDS::savePresetAsJson()
{
	if (!dataLoaded)
	{
		QMessageBox::critical(this, tr("Error"), tr("You must load any data before save."), tr("OK"));
		return;
	}

	QString filename(QFileDialog::getSaveFileName(this, tr("Save Preset"), "./", tr("Json File(*.json)")));
	if (!filename.isEmpty())
	{
		PresetSaver saver;
		if (!saver.savePreset(data, filename))
		{
			QMessageBox::critical(this, tr("Error"), tr("Failed to save preset as json."), tr("OK"));
		}
	}
}

void PngPortrait2DDS::loadPresetFromJson(const QString& file)
{
	PresetLoader loader;
	auto [preset, err] = loader.loadFromJson(file);

	if (!loader.success())
	{
		QMessageBox::critical(this, tr("Error"), tr("Invalid Json File.") + loader.errorString(), tr("OK"));
		return;
	}
	data = preset;

	python.setEnabledScript(data.pyScript());
	ui->statusBar->showMessage(tr("Python script has been set to %1  ").arg(python.enabledScript()), 5000);
	ui->tbwPngItems->loadPortraitsInfo(data);
}


void PngPortrait2DDS::resizeEvent(QResizeEvent* evt)
{
	this->onImageSizeChanged(0);
}

void PngPortrait2DDS::dragEnterEvent(QDragEnterEvent* evt)
{
	int cmp = evt->mimeData()->urls().at(0).fileName().right(4).compare("json", Qt::CaseSensitivity::CaseInsensitive);
	if (cmp == 0)
		evt->acceptProposedAction();
	else
		evt->ignore();
}
void PngPortrait2DDS::dropEvent(QDropEvent* evt)
{
	this->loadPresetFromJson(evt->mimeData()->urls().at(0).toLocalFile());
}