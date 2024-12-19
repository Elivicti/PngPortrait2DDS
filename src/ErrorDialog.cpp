#include "ErrorDialog.h"
#include "ui/ui_ErrorDialog.h"

#include <QFile>

ErrorDialog::ErrorDialog(const QString& title, const QString& text, Icon icon, QWidget* parent)
	: QDialog{ parent }
	, ui{ new Ui::ErrorDialog{} }
{
	ui->setupUi(this);

	if (icon != Icon::NoIcon)
	{
		QIcon critical_icon = style()->standardIcon((QStyle::StandardPixmap)(icon));
		ui->lbIcon->setPixmap(critical_icon.pixmap(QSize{ 48, 48 }));
	}
	else
	{
		ui->lbIcon->setText("");
	}

	this->setWindowTitle(title);
	ui->teMessage->setPlainText(text);

	connect(ui->btnOK, &QPushButton::clicked, this, &QDialog::accept);
}

ErrorDialog::~ErrorDialog()
{
	delete ui;
}

void ErrorDialog::setHelperText(const QString& helper_text)
{
	ui->lbHelperText->setText(helper_text);
}