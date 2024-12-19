#pragma once

#include <QDialog>
#include <QStyle>

QT_BEGIN_NAMESPACE
namespace Ui { class ErrorDialog; };
QT_END_NAMESPACE

class ErrorDialog : public QDialog
{
	Q_OBJECT

public:
	enum class Icon
	{
		NoIcon,
		Critical = QStyle::SP_MessageBoxCritical,
		Warning = QStyle::SP_MessageBoxWarning
	};

public:
	ErrorDialog(const QString& title, const QString& text, Icon icon = Icon::Critical, QWidget* parent = nullptr);
	~ErrorDialog();

	void setHelperText(const QString& helper_text);

private:
	Ui::ErrorDialog *ui;
};
