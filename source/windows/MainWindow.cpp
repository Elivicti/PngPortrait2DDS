#include "windows/MainWindow.h"
#include "ui/ui_MainWindow.h"

#include "utils/QtFileSystem.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow{ parent }
	, ui{ new Ui::MainWindow{} }
{
	ui->setupUi(this);

}

MainWindow::~MainWindow() {}
