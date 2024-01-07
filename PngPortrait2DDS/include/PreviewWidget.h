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
	QPoint getImageOffset() const { return pngOffset; }

	void setImageOffsetX(int x) { pngOffset.setX(x); }
	void setImageOffsetY(int y) { pngOffset.setY(y); }
	void setImageOffset(const QPoint& offset) { pngOffset = offset; }
	void setImageOffset(int x, int y)
	{
		pngOffset.setX(x);
		pngOffset.setY(y);
	}

	bool isCursorMovingImage() const { return movingImage; }
	
	void scaledResize(int w, int h,const QSize& imgSize);

signals:
	void imageOffsetChangedByCursor(const QPoint& p);
	void imageScaleChangedByWheel(double ratio);

public slots:
	void scalePng(double ratio);

protected:
	virtual void paintEvent(QPaintEvent*)			override;

	virtual void mousePressEvent(QMouseEvent*)		override;
	virtual void mouseMoveEvent(QMouseEvent*)		override;
	virtual void mouseReleaseEvent(QMouseEvent*)	override;

	virtual void wheelEvent(QWheelEvent*)			override;

private:
	inline void scalePng() {
		scaledPng = png.scaled(
			png.size() * scaleRatio * widgetScale,
			Qt::AspectRatioMode::KeepAspectRatio,
			Qt::TransformationMode::SmoothTransformation
		);
	}

private:
	double scaleRatio;
	double widgetScale;

	QPixmap png;
	QPixmap scaledPng;
	QPoint pngOffset;
	QPoint cursorOffset;

	bool movingImage;
};
