#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PngPortrait2DDS.h"
#include "PortraitDataList.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PngPortrait2DDSClass; };
QT_END_NAMESPACE

#include <QFileInfo>
#include <QProcess>


class PngPortrait2DDS : public QMainWindow
{
	Q_OBJECT

public:
	PngPortrait2DDS(QWidget* parent = nullptr);
	~PngPortrait2DDS();

public slots:
	void show();

private slots:
	void onBrowseDirectoryClicked();
	void onTableLoadingCompleted();

	void onPortraitSelected(const QModelIndex& index);
	void onUseSeperateSettingStateChanged(int state);

	void onImageSizeChanged(int value);
	void onImageOffsetXChanged(int value);
	void onImageOffsetYChanged(int value);


	void onExportOptionChanged(int state);
	void onExport();

	void savePresetAsJson();
	void loadPresetFromJson(const QString& file);

protected:
	virtual void resizeEvent(QResizeEvent* evt) override;

	virtual void dragEnterEvent(QDragEnterEvent* evt) override;
	virtual void dropEvent(QDropEvent* evt) override;

private:
	void disableEditorWidget();
	void enableEditorWidget();

private:
	Ui::PngPortrait2DDSClass* ui;
	QLabel* lbLog;
	bool dataLoaded;

	PortraitDataList data;
	int currIndex;
};
