#pragma once

#include <QWidget>
#include <QPixmap>

class PreviewWidget  : public QWidget
{
	Q_OBJECT

public:
	PreviewWidget(QWidget *parent);
	~PreviewWidget();

	bool setPreviewPng(const QString& filepath);

	double getScaleRatio() const { return scaleRatio; }

public slots:
	void scalePng(double ratio);

protected:
	virtual void paintEvent(QPaintEvent* evt) override;

private:
	double scaleRatio;
	QPixmap png;
	QPixmap scaledPng;
};
