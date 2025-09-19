#pragma once

#include <QGridLayout>


class KeepAspectRatioLayout : public QGridLayout
{
	Q_OBJECT

public:
	KeepAspectRatioLayout(QWidget* parent = nullptr);

	void setAspectRatio(double ratio) { aspect_ratio = ratio; }
	void setAspectRatio(QSizeF size) { aspect_ratio = (double)size.width() / (double)size.height(); }
	void setAspectRatio(QSize  size) { this->setAspectRatio(size.toSizeF()); }
	void setAspectRatio(int width, int height) { aspect_ratio = (double)width / (double)height; }
	double aspectRatio() const { return aspect_ratio; }

	void addWidget(QWidget* w)
	{ QGridLayout::addWidget(w); }

	void addWidget(QWidget* w, int row, int col)
	{ QGridLayout::addWidget(w, row, col); }

public:
	virtual void setGeometry(const QRect& r) override;

private:
	void addWidget(QWidget* w, int row, int col, int row_span, int col_span, Qt::Alignment align = Qt::Alignment{})
	{
		QGridLayout::addWidget(w, row, col, row_span, col_span, align);
	}

private:
	double aspect_ratio;
};
