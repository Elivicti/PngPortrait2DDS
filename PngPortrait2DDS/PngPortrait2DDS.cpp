#include "PngPortrait2DDS.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QStandardItemModel>
#include <QDir>
#include <QPainter>
#include <QMessageBox>

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

	connect(ui->cbExportDDS, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onExportOptionChanged);
	connect(ui->cbExportRegistration, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onExportOptionChanged);
	connect(ui->cbExportProperNameEffect, &QCheckBox::stateChanged, this, &PngPortrait2DDS::onExportOptionChanged);

	connect(ui->btnExport, &QPushButton::clicked, this, &PngPortrait2DDS::onExport);

	this->disableEditorWidget();
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

		dataLoaded = true;
		this->enableEditorWidget();
		ui->wPreview->scalePng(data.defaultScale());
		ui->dspbPotraitScale->setValue(data.defaultScale());
	}
}

void PngPortrait2DDS::onPortraitSelected(const QModelIndex& index)
{

	if (index.isValid())
	{
		currIndex = index.row();

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
	onExportOptionChanged(0);
}

void PngPortrait2DDS::onExportOptionChanged(int state)
{
	bool anyOptionEnable = ui->cbExportDDS->isChecked() 
						|| ui->cbExportRegistration->isChecked() 
						|| ui->cbExportProperNameEffect->isChecked();
	ui->btnExport->setEnabled(anyOptionEnable && dataLoaded);
}

void PngPortrait2DDS::onExport()
{
	QSize imgSize = ui->wPreview->size();
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
	
	bool export_dds = ui->cbExportDDS->isChecked();
	bool export_registration = ui->cbExportRegistration->isChecked();
	bool export_effect = ui->cbExportProperNameEffect->isChecked();
	
	// initialize base image
	QPixmap img(imgSize);

	// create file structure
	QDir output(data.at(0)->portrait.absoluteDir());
	QString output_name = output.dirName() + QString("_output");
	QString dds_path = output.dirName();
	output.cdUp();
	output.mkdir(output_name);
	output.cd(output_name);

	if (export_dds)
	{
		output.mkdir(dds_path);
	}

	QFile registration;
	struct
	{
		QString dds_path;
		QString game_setup;
		QString species;
		QString& pop = species;
		QString leader;
		QString ruler;
	} reg_info;
	if (export_registration)
	{
		registration.setFileName(output.absoluteFilePath(dds_path + ".txt"));
		registration.open(QIODevice::ReadWrite | QIODevice::Text | QIODeviceBase::Truncate);
		
	}

	QFile effect;
	QFile yml;
	QTextStream yml_stream(&yml);
	if (export_effect)
	{
		effect.setFileName(output.absoluteFilePath(dds_path + "_name_effect.txt"));
		effect.open(QIODevice::ReadWrite | QIODevice::Text | QIODeviceBase::Truncate);
		effect.write("random_list = {\n");

		yml.setFileName(output.absoluteFilePath(dds_path + "_name_effect.yml"));
		yml.open(QIODevice::ReadWrite | QIODevice::Text | QIODeviceBase::Truncate);
		yml_stream.setGenerateByteOrderMark(true);
		yml_stream << "l_simp_chinese:\n";
	}
	
	// In the end, file structure will be something like this:
	//  path/							// where png files are stored
	//  path_output/					// where all generated files are
	//		©À©¤path/						// generated dds goes here    (if enabled)
	//		©À©¤registration.txt			// portrait registration file (if enabled)
	//		©À©¤effect.txt				// portrait's proper name effect (if enabled)
	//		©¸©¤names.yml					// portrait's proper name effect (if enabled)

	qsizetype portraitCnt = data.size();
	QString targetPng(output.absoluteFilePath("output.png").replace("/", "\\"));
	
	lbLog->setText(tr("Processing..."));

	for (qsizetype i = 0; i < portraitCnt; i++)
	{
		QFileInfo& file = data.at(i)->portrait;
		QFileInfo targetDDS(output.absoluteFilePath(dds_path + "/" + file.fileName().replace(".png", ".dds")));

		if (export_dds)
		{
			img.fill(QColor(0, 0, 0, 0));

			// load portrait
			QPixmap portrait(file.absoluteFilePath());

			// read preset
			QPoint offset = data.offset(i);
			double scale = data.scale(i);

			// draw & save
			QPainter painter(&img);
			painter.drawPixmap(offset, 
				portrait.scaled(portrait.size() * scale, 
					Qt::AspectRatioMode::KeepAspectRatio, 
					Qt::TransformationMode::SmoothTransformation
			));

			img.save(targetPng);

			QStringList args;
			args << "-dxt3"
				<< "-file" << targetPng
				<< "-alpha"
				<< "-output" << targetDDS.absoluteFilePath().replace("/", "\\");

			// png2dds command line
			// nvdxt.exe -file input.png -alpha [-scale 2] -output output.dds
			nvdxt->start("./nvdxt.exe", args);
			nvdxt->waitForFinished();
		}
		
		if (export_registration)
		{
			QString portrait_id(QString("%1_%2").arg(dds_path).arg(QString::number(i + 1), 3, QChar('0')));

			QString reg("\t{ %1 = \"gfx/models/portraits/%2/%3\" }\n");

			reg_info.dds_path += reg.arg(portrait_id).arg(dds_path).arg(targetDDS.fileName());

			((reg_info.game_setup += "\t\t\t\t\t") += portrait_id) += "\n";
			
			if (ui->tbvPngItems->isUsingPortraitType(i, PortraitUsingType::Species))
				((reg_info.species += "\t\t\t\t\t") += portrait_id) += "\n";
			if (ui->tbvPngItems->isUsingPortraitType(i, PortraitUsingType::Leader))
				((reg_info.leader += "\t\t\t\t\t") += portrait_id) += "\n";
			if (ui->tbvPngItems->isUsingPortraitType(i, PortraitUsingType::Ruler))
				((reg_info.ruler += "\t\t\t\t\t") += portrait_id) += "\n";
		}

		if (export_effect)
		{
			QString portrait_id(QString("%1_%2").arg(dds_path).arg(QString::number(i + 1), 3, QChar('0')));

			QString dds_name(targetDDS.completeBaseName());
			yml_stream << " NAME_" << dds_name << ":0 \"" << dds_name << "\"\n";

			QString effectline(QString("\t1 = { portrait = %1  set_name = %2 }\n"));
			effect.write(effectline.arg(portrait_id).arg("NAME_" + dds_name).toUtf8());
		}
	}

	if (export_dds)
	{
		QFile(targetPng).remove();
	}

	if (export_registration)
	{
		
		registration.write("portraits = {\n");
		registration.write(reg_info.dds_path.toUtf8());
		registration.write("}\n");
		registration.write(QString(
			"portrait_groups = {\n"
			"\t%1 = {\n"
			"\t\tdefault = %2\n").arg(dds_path).arg(dds_path + "_001").toUtf8());
#define WritePortraitInfo(reg, key) \
	reg.write("\t\t"#key" = {\n\t\t\tadd = {\n\t\t\t\tportraits = {\n");	\
	reg.write(reg_info.key.toUtf8());	\
	reg.write("\t\t\t\t}\n\t\t\t}\n\t\t}\n");

		WritePortraitInfo(registration, game_setup);
		WritePortraitInfo(registration, species);
		WritePortraitInfo(registration, pop);
		WritePortraitInfo(registration, leader);
		WritePortraitInfo(registration, ruler);

		registration.write("\t}\n}");
		registration.close();

#undef WritePortraitInfo
	}

	if (export_effect)
	{
		yml.close();
		effect.write("}");
		effect.close();
	}

	lbLog->setText(tr("Completed."));
}

void PngPortrait2DDS::resizeEvent(QResizeEvent* evt)
{
	this->onImageSizeChanged(0);
}