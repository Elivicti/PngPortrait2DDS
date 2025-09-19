#include "windows/MainWindow.h"
#include "ui/ui_MainWindow.h"

#include "utils/QtFileSystem.h"
#include "utils/misc.h"

#include <QFileDialog>

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow{ parent }
	, ui{ new Ui::MainWindow{} }
{
	ui->setupUi(this);

	ui->spbOffsetX->setMinimum(std::numeric_limits<int>::min());
	ui->spbOffsetX->setMaximum(std::numeric_limits<int>::max());

	ui->spbOffsetY->setMinimum(ui->spbOffsetX->minimum());
	ui->spbOffsetY->setMaximum(ui->spbOffsetX->maximum());


	ui->splitter_main->setStretchFactor(0, 0);
	ui->splitter_main->setStretchFactor(1, 1);

	ui->splitter_side->setStretchFactor(0, 1);
	ui->splitter_side->setStretchFactor(1, 0);

	ui->splitter_config->setStretchFactor(0, 0);
	ui->splitter_config->setStretchFactor(1, 1);

	ui->gbPortraitConfig->setMinimumWidth(ui->gbImageSize->minimumWidth());

	ui->treePictures->clear();

	ui->gbPreviewContainer->setPreviewWidget(ui->picView);
	ui->gbPreviewContainer->setViewSize(QSize{ 496, 380 });

	ui->picView->setScale(0.5);
	ui->picView->setAcceptWheel(0.05);
	ui->picView->setAcceptDrag();
	ui->picView->setWheelScaleCenterAtCursorPos(true);

	connect(ui->actionOpen, &QAction::triggered, [this]() {
		QString name = QFileDialog::getExistingDirectory(this);
		qDebug() << name;
		if (name.isEmpty())
			return;

		ui->treePictures->addDirectory(name);
	});

	connect(ui->treePictures, &PortraitTreeWidget::portraitSelected, [this](PortraitItem* item) {
		ui->picView->loadPicture(item->path());
		ui->picView->update();
	});


	connect(ui->picView, &PictureView::offsetChanged, [this](QPoint offset) {
		SignalBlockerGuard x_guard{ ui->spbOffsetX };
		SignalBlockerGuard y_guard{ ui->spbOffsetY };

		ui->spbOffsetX->setValue(offset.x());
		ui->spbOffsetY->setValue(offset.y());

	});

	connect(ui->spbOffsetX, &QSpinBox::valueChanged, [this](int offset_x) {
		SignalBlockerGuard guard{ ui->picView };

		ui->picView->setOffset(offset_x, ui->picView->offset().y());
		ui->picView->update();
	});
	connect(ui->spbOffsetY, &QSpinBox::valueChanged, [this](int offset_y) {
		SignalBlockerGuard guard{ ui->picView };

		ui->picView->setOffset(ui->picView->offset().x(), offset_y);
		ui->picView->update();
	});
}

MainWindow::~MainWindow() {}
