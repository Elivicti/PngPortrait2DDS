#include "PreviewWidget.h"

#include <QPaintEvent>
#include <QPainter>

#include <qdebug.h>

PreviewWidget::PreviewWidget(QWidget *parent)
	: QWidget(parent)
	, scaleRatio(1.0), widgetScale(1.0), pngOffset(0, 0)
	, movingImage(false)
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
		painter.drawPixmap(pngOffset.x() * widgetScale, pngOffset.y() * widgetScale, scaledPng);
	}

	// Frame
	painter.setPen(QColor(0, 0, 0));
	painter.drawRect(0, 0, this->width(), this->height());
}


void PreviewWidget::mousePressEvent(QMouseEvent* evt) {
	movingImage = (evt->button() == Qt::MouseButton::LeftButton) && !png.isNull();
	if (movingImage)
	{
		cursorOffset = pngOffset - evt->pos();
	}
}
void PreviewWidget::mouseMoveEvent(QMouseEvent* evt) {
	if (movingImage)
	{
		pngOffset = evt->pos() + cursorOffset;
		this->repaint();

		emit imageOffsetChangedByCursor(pngOffset);
	}
}
void PreviewWidget::mouseReleaseEvent(QMouseEvent* evt)
{
	movingImage = false;
}

void PreviewWidget::wheelEvent(QWheelEvent* evt)
{
	if (!png.isNull())
	{
		if (evt->angleDelta().y() > 0 && scaleRatio < 2.0 - 1e-6)
			scaleRatio += 0.01;
		else if (evt->angleDelta().y() < 0 && scaleRatio > 0.0)
			scaleRatio -= 0.01;

		scalePng();
		this->repaint();
		
		emit imageScaleChangedByWheel(scaleRatio);
	}
}

bool PreviewWidget::setPreviewPng(const QString& filepath)
{
	if (png.load(filepath))
	{
		scalePng();
		this->repaint();
		return true;
	}
	else
		return false;
}

void PreviewWidget::scaledResize(int w, int h, const QSize& imgSize)
{
	widgetScale = (double)h / (double)imgSize.height();

	if (!scaledPng.isNull())
		scalePng();

	QWidget::resize(w, h);
}

// public slots
void PreviewWidget::scalePng(double ratio)
{
	scaleRatio = ratio;
	if (!png.isNull())
	{
		scalePng();
	}
}