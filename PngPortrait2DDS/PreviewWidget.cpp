#include "PreviewWidget.h"

#include <QPainter>

#include <qdebug.h>

PreviewWidget::PreviewWidget(QWidget *parent)
	: QWidget(parent)
	, scaleRatio(1.0)
{
}

PreviewWidget::~PreviewWidget()
{}

void PreviewWidget::paintEvent(QPaintEvent* evt)
{
	//QWidget::paintEvent(evt);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::TextAntialiasing);

	if (!png.isNull())
	{
		painter.drawPixmap(0, 0, scaledPng);
	}
}

bool PreviewWidget::setPreviewPng(const QString& filepath)
{
	if (png.load(filepath))
	{
		scaledPng = png.scaled(png.size() * scaleRatio, Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
		this->repaint();

		return true;
	}
	else
		return false;
}

// public slots
void PreviewWidget::scalePng(double ratio)
{
	scaleRatio = ratio;
	if (!png.isNull())
	{
		scaledPng = png.scaled(
			png.size() * scaleRatio,
			Qt::AspectRatioMode::KeepAspectRatio,
			Qt::TransformationMode::SmoothTransformation
		);
	}
}