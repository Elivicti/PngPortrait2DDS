#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PngPortrait2DDS.h"

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

    void onImageSizeChanged(int value);
    void onImageOffsetChanged(int value);

    void onExport();

protected:
    virtual void resizeEvent(QResizeEvent* evt) override;

private:
    Ui::PngPortrait2DDSClass* ui;
    QLabel* lbLog;

    QFileInfoList pngSources;

    QProcess* nvdxt;
};
