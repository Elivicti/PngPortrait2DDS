#include "PngPortrait2DDS.h"

#include <QFileDialog>
#include <QFileInfo>
#include <QDir>
#include <QPainter>
#include <QMessageBox>
#include <QtConcurrent>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include <QDropEvent>
#include <QMimeData>

#include <SOIL2/SOIL2.h>

#include <QDebug>

#define ScaleSliderMaxValue 1000
#define DefaultScale 0.20

PngPortrait2DDS::PngPortrait2DDS(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::PngPortrait2DDSClass()), lbLog(new QLabel(this)), dataLoaded(false)
	, data(0, 0, DefaultScale), currIndex(-1)
{
	ui->setupUi(this);
	ui->statusBar->addPermanentWidget(lbLog);

	connect(ui->btnBrowseDirectory, &QPushButton::clicked, this, &PngPortrait2DDS::onBrowseDirectoryClicked);

	connect(ui->tbwPngItems, &PortraitTable::tableLoadingCompleted, this, &PngPortrait2DDS::onTableLoadingCompleted);

	connect(ui->tbwPngItems, &QAbstractItemView::activated, this, &PngPortrait2DDS::onPortraitSelected);

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

	connect(ui->actionSavePreset, &QAction::triggered, this, &PngPortrait2DDS::savePresetAsJson);
	connect(ui->actionLoadPreset, &QAction::triggered, [this]() {
		QString filename(QFileDialog::getOpenFileName(this, tr("Select Preset"), "./", tr("Json File(*.json)")));
		if (!filename.isEmpty())
			this->loadPresetFromJson(filename);
	});

	this->setAcceptDrops(true);
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

		data.clear();
		ui->lePngDirectory->setText(path.replace("/", "\\"));
		QDir dir(path);
		QStringList filter(QString("*.png"));
		QFileInfoList pngSources = dir.entryInfoList(filter);
		for (auto& png : pngSources)
			data.append(new PortraitData(png));

		ui->tbwPngItems->setPortraitsInfo(dir.entryList(filter));
	}
}

void PngPortrait2DDS::onTableLoadingCompleted()
{
	ui->statusBar->showMessage(tr("Loading Completed: %1 Total").arg(data.size()));

	dataLoaded = true;
	this->enableEditorWidget();
	ui->wPreview->scalePng(data.defaultScale());
	ui->dspbPotraitScale->setValue(data.defaultScale());
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
		ui->cbExportProperNameEffect->setDisabled(true);
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
		ui->cbExportProperNameEffect->setEnabled(true);
		ui->menuOptions->setEnabled(true);
	}
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

	auto func_export = [this, imgSize]() {
		QProcess* nvdxt = new QProcess();
		
		qsizetype portraitCnt = data.size();
		bool export_dds = ui->cbExportDDS->isChecked();
		bool export_registration = ui->cbExportRegistration->isChecked();
		bool export_effect = ui->cbExportProperNameEffect->isChecked();

		// initialize base image
		QImage img(imgSize, QImage::Format_RGBA8888);

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
			yml_stream.setGenerateByteOrderMark(true);	// UTF8 With BOM
			yml_stream << "l_simp_chinese:\n";
		}

		// In the end, file structure will be something like this:
		// path/							// where png files are stored
		// path_output/						// where all generated files are
		//  ©À©¤path/							// generated dds goes here    (if enabled)
		//	©À©¤registration.txt				// portrait registration file (if enabled)
		//	©À©¤effect.txt					// portrait's proper name effect (if enabled)
		//	©¸©¤names.yml						// portrait's proper name effect (if enabled)

		
		QString targetPng(output.absoluteFilePath("output.png").replace("/", "\\"));
		QStringList args({"-dxt3", "-file", targetPng, "-alpha", "-output"});
		/*args << "-dxt3"
			<< "-file" << targetPng
			<< "-alpha"
			<< "-output";*/

		for (qsizetype i = 0; i < portraitCnt; i++)
		{
			lbLog->setText(tr("Processing (%1/%2)...").arg(i).arg(portraitCnt));

			QFileInfo& file = data.at(i)->portrait;
			QFileInfo targetDDS(output.absoluteFilePath(dds_path + "/" + file.fileName().replace(".png", ".dds")));

			if (export_dds)
			{
				img.fill(QColor(0, 0, 0, 0));

				// load portrait
				QImage portrait(file.absoluteFilePath());

				// read preset
				QPoint offset = data.offset(i);
				double scale = data.scale(i);

				// draw & save
				QPainter painter(&img);

				painter.drawImage(
					offset,
					portrait.scaled(
						portrait.size()* scale,
						Qt::AspectRatioMode::KeepAspectRatio,
						Qt::TransformationMode::SmoothTransformation
					)
				);

				/* reminder
				QImage *pTexture = [Your initial image]

				QImage imgProperlyFormatted = pTexture->convertToFormat(QImage::Format_RGBA8888);
				SOIL_save_image_quality(
					"MyTexture.dds",
					SOIL_SAVE_TYPE_DDS,
					imgProperlyFormatted.width(),
					imgProperlyFormatted.height(),
					4,
					imgProperlyFormatted.bits(),
					0
				);
				*/

				// TODO:
				// make conversion from png to dds in multithreads

				// save as dxt5
				SOIL_save_image_quality(
					"",
					SOIL_SAVE_TYPE_DDS,
					img.width(),
					img.height(),
					4,
					img.bits(),
					0
				);

				//img.save(targetPng);

				//args << targetDDS.absoluteFilePath().replace("/", "\\");

				// png2dds command line
				// nvdxt.exe -file input.png -alpha [-scale 2] -output output.dds
				//nvdxt->start("./nvdxt.exe", args);
				//nvdxt->waitForFinished();

				//args.pop_back();
			}

			if (export_registration)
			{
				QString portrait_id(QString("%1_%2").arg(dds_path).arg(QString::number(i + 1), 3, QChar('0')));

				QString reg("\t%1 = { texturefile = \"gfx/models/portraits/%2/%3\" }\n");

				reg_info.dds_path += reg.arg(portrait_id).arg(dds_path).arg(targetDDS.fileName());

				((reg_info.game_setup += "\t\t\t\t\t") += portrait_id) += "\n";

				if (ui->tbwPngItems->isUsingPortraitType(i, PortraitUsingType::Species))
					((reg_info.species += "\t\t\t\t\t") += portrait_id) += "\n";
				if (ui->tbwPngItems->isUsingPortraitType(i, PortraitUsingType::Leader))
					((reg_info.leader += "\t\t\t\t\t") += portrait_id) += "\n";
				if (ui->tbwPngItems->isUsingPortraitType(i, PortraitUsingType::Ruler))
					((reg_info.ruler += "\t\t\t\t\t") += portrait_id) += "\n";
			}

			if (export_effect)
			{
				QString portrait_id(QString("%1_%2").arg(dds_path).arg(QString::number(i + 1), 3, QChar('0')));

				QString dds_name(targetDDS.completeBaseName());
				yml_stream << " NAME_" << dds_name << ":0 \"" << dds_name << "\"\n";

				QString effectline(QString("\t1 = { portrait = %1  set_name = NAME_%2  set_leader_flag = flag_%2 }\n"));
				effect.write(effectline.arg(portrait_id).arg(dds_name).toUtf8());
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
				"\t\tdefault = %2\n"
			).arg(dds_path).arg(dds_path + "_001").toUtf8());
#define WritePortraitInfo(reg, key)											\
	reg.write("\t\t"#key" = {\n\t\t\tadd = {\n\t\t\t\tportraits = {\n");	\
	reg.write(reg_info.key.toUtf8());										\
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

		delete nvdxt;
	};

	this->disableEditorWidget(true);
	QFutureWatcher<void>* watcher = new QFutureWatcher<void>;
	connect(watcher, &QFutureWatcher<void>::finished, [this, watcher]() {
		lbLog->setText(tr("Completed."));

		watcher->deleteLater();
		this->enableEditorWidget(true);
	});

	QFuture<void> future = QtConcurrent::run(func_export);
	watcher->setFuture(future);
	
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
		QJsonObject obj;

		QJsonValue path(ui->lePngDirectory->text());

		QJsonObject setting;
		QJsonArray imgSize({ ui->spbImageWidth->value(), ui->spbImageHeight->value() });
		QJsonArray defaultOffset({ data.defaultOffset().x(), data.defaultOffset().y() });
		QJsonValue defaultScale(data.defaultScale());
		setting.insert("Size", imgSize);
		setting.insert("Offset", defaultOffset);
		setting.insert("Scale", defaultScale);

		QJsonObject exportOptions;
		exportOptions.insert("DDS", QJsonValue(ui->cbExportDDS->isChecked()));
		exportOptions.insert("Registration", QJsonValue(ui->cbExportRegistration->isChecked()));
		exportOptions.insert("Proper Name Effect", QJsonValue(ui->cbExportProperNameEffect->isChecked()));

		QJsonArray portraits;
		qsizetype cnt = data.size();
		for (qsizetype i = 0; i < cnt; i++)
		{
			QJsonObject p;
			p.insert("Use Types", QJsonArray({
					ui->tbwPngItems->isUsingPortraitType(i, PortraitUsingType::Species),
					ui->tbwPngItems->isUsingPortraitType(i, PortraitUsingType::Leader),
					ui->tbwPngItems->isUsingPortraitType(i, PortraitUsingType::Ruler)
				}));
			bool useSeperateSetting = data.at(i)->useSeperateSetting;
			p.insert("Use Seperate Setting", QJsonValue(useSeperateSetting));
			if (useSeperateSetting)
			{
				p.insert("Offset", QJsonArray({
					data.offset(i).x(),
					data.offset(i).y()
					}));
				p.insert("Scale", QJsonValue(data.scale(i)));
			}
			portraits.append(p);
		}

		obj.insert("Path", path);
		obj.insert("Default Setting", setting);
		obj.insert("Export Options", exportOptions);
		obj.insert("Portraits", portraits);

		QFile json(filename);
		json.open(QIODevice::ReadWrite | QIODevice::Text | QIODeviceBase::Truncate);
		json.write(QJsonDocument(obj).toJson());

		json.close();
	}
}

void PngPortrait2DDS::loadPresetFromJson(const QString& file)
{
	while (!file.isEmpty())
	{
		currIndex = -1;

		QFile json(file);
		json.open(QIODevice::ReadOnly | QIODevice::Text);
		QJsonDocument doc(QJsonDocument::fromJson(json.readAll()).object());
		if (doc.isNull())
			break;

		QJsonObject obj(doc.object());
		json.close();

		QString path(obj.value("Path").toString());
		QDir dir(path);
		if (!dir.exists())
			break;

		QJsonObject defaultSetting(obj.value("Default Setting").toObject());
		if (defaultSetting.isEmpty())
			break;
		QJsonValue scale(defaultSetting.value("Scale"));
		QJsonArray offset(defaultSetting.value("Offset").toArray());
		QJsonArray imgSize(defaultSetting.value("Size").toArray());

		QJsonObject exportOptions(obj.value("Export Options").toObject());
		if (exportOptions.isEmpty())
			break;

		if (!scale.isNull())
		{
			data.setDefaultScale(scale.toDouble());
			ui->dspbPotraitScale->setValue(data.defaultScale());
		}
		if (!offset.isEmpty())
		{
			data.setDefaultOffset(offset.at(0).toInt(), offset.at(1).toInt());
			ui->spbPortraitOffsetX->setValue(offset.at(0).toInt());
			ui->spbPortraitOffsetY->setValue(offset.at(1).toInt());
		}
		if (!imgSize.isEmpty())
		{
			ui->spbImageWidth->setValue(imgSize.at(0).toInt());
			ui->spbImageHeight->setValue(imgSize.at(1).toInt());
		}
		

		ui->cbExportDDS->setChecked(exportOptions.value("DDS").toBool());
		ui->cbExportRegistration->setChecked(exportOptions.value("Registration").toBool());
		ui->cbExportProperNameEffect->setChecked(exportOptions.value("Proper Name Effect").toBool());


		QJsonArray portraits(obj.value("Portraits").toArray());
		data.clear();
		ui->tbwPngItems->clearContents();
		ui->lePngDirectory->setText(path);
		QStringList filter(QString("*.png"));
		QFileInfoList pngSources = dir.entryInfoList(filter);

		if (portraits.isEmpty())
		{
			for (auto& png : pngSources)
				data.append(new PortraitData(png));
			ui->tbwPngItems->setPortraitsInfo(dir.entryList(filter));
		}
		else
		{
			int cnt = pngSources.size();
			for (int i = 0; i < cnt; i++)
			{
				data.append(new PortraitData(pngSources[i]));
				QJsonObject portrait(portraits.at(i).toObject());
				if (!portrait.isEmpty())
				{
					data.at(i)->useSeperateSetting = portrait.value("Use Seperate Setting").toBool();
					if (data.at(i)->useSeperateSetting)
					{
						QJsonArray offset(portrait.value("Offset").toArray());
						data.at(i)->offset.setX(offset[0].toInt());
						data.at(i)->offset.setY(offset[1].toInt());
						data.at(i)->scale = portrait.value("Scale").toDouble();
					}

					QJsonArray usingTypes(portrait.value("Use Types").toArray());
					ui->tbwPngItems->appendPortraitInfo(
						pngSources[i].fileName(),
						usingTypes[0].toBool(),
						usingTypes[1].toBool(),
						usingTypes[2].toBool()
					);
				}
				else
					ui->tbwPngItems->appendPortraitInfo(pngSources[i].fileName(), true, true, true);
			}
		}

		emit ui->tbwPngItems->tableLoadingCompleted();

		dataLoaded = true;
		this->enableEditorWidget();

		return;
	}

	QMessageBox::critical(this, tr("Error"), tr("Invalid Json File."), tr("OK"));
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