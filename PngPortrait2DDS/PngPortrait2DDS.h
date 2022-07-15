#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_PngPortrait2DDS.h"

QT_BEGIN_NAMESPACE
namespace Ui { class PngPortrait2DDSClass; };
QT_END_NAMESPACE

#include <QFileInfo>

class PngPortrait2DDS : public QMainWindow
{
    Q_OBJECT

public:
    PngPortrait2DDS(QWidget* parent = nullptr);
    ~PngPortrait2DDS();

private slots:
    void onBrowsDirectoryClicked();

private:
    Ui::PngPortrait2DDSClass* ui;

    QFileInfoList pngSources;
};
