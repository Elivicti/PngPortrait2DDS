#pragma once

#include <QFrame>

class PictureView : public QFrame
{
	Q_OBJECT

public:
	PictureView(QWidget* parent = nullptr);
	~PictureView();

	void loadPicture(const QString& path, const char* format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor)
	{ data.picture.load(path, format, flags); }

	void loadPictureFromData(const QByteArray& buf, const char* format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor)
	{ data.picture.loadFromData(buf, format, flags); }
	void loadPictureFromData(const unsigned char* buf, std::size_t size, const char* format = nullptr, Qt::ImageConversionFlags flags = Qt::AutoColor)
	{ data.picture.loadFromData(buf, size, format, flags); }

	void setPicture(const QPixmap& pixmap)
	{ data.picture = pixmap; }
	void setPicture(const QImage&  img, Qt::ImageConversionFlags flags)
	{ data.picture.convertFromImage(img, flags); }

	void unload()
	{ data.picture = QPixmap{}; }

	QPixmap pixmap() const { return data.picture; }

	QPoint offset() const { return data.offset; }
	void setOffset(QPoint p)
	{
		QPoint old = data.offset;
		data.offset = p;
		Q_EMIT offsetChanged(data.offset, old);
	}
	void setOffset(int x, int y)
	{ setOffset(QPoint{ x, y }); }

	double scale() const { return data.scale; }
	void setScale(double scale)
	{
		double old = data.scale;
		data.scale = std::max({ scale, 0.0 });

		if (data.scale == old)
			return;
		Q_EMIT scaleChanged(data.scale, old);
	}

	double zoom() const { return data.zoom_factor; }
	void setZoom(double zoom)
	{
		data.zoom_factor = std::max({ zoom, 0.0 });
		this->force_repaint();
	}

	// Set to Qt::NoButton to disable mouse drag.
	void setAcceptDrag(Qt::MouseButton button = Qt::LeftButton) { drag_button = button; }
	bool acceptDrag() const { return drag_button == Qt::NoButton; }

	// Set to less than or equal to 0 to disable wheel scale.
	void setAcceptWheel(double step = 0.1) { wheel_step = step; }
	bool acceptWheel() const { return wheel_step > 0.0; }

	void setWheelScaleCenterAtCursorPos(bool b)
	{ cursor_as_whl_scale_center = b; }

	void setTrackMouseDrag(bool b)
	{ track_mouse_drag = b; }

	bool isMouseDragging() const
	{ return state.dragging; }

	bool isWheelScaling() const
	{ return state.scaling; }

Q_SIGNALS:
	void offsetChanged(QPoint new_offset, QPoint old_offset);
	void scaleChanged(double new_scale, double old_scale);

protected:
	void paintEvent(QPaintEvent* evt) override;

	void mousePressEvent(QMouseEvent* evt) override;
	void mouseMoveEvent(QMouseEvent* evt) override;
	void mouseReleaseEvent(QMouseEvent* evt) override;
	void wheelEvent(QWheelEvent* evt) override;

private Q_SLOTS:
	void force_repaint()
	{
		if (!state.no_force_repaint)
			this->update();
	}

private:
	struct
	{
		QPixmap picture;
		QPoint offset;
		double scale;
		double zoom_factor;
	} data;

	Qt::MouseButton drag_button;
	double wheel_step;
	bool cursor_as_whl_scale_center;
	bool track_mouse_drag;

	struct
	{
		bool dragging;
		bool scaling;

		QPoint old_offset;
		QPoint cursor_offset;

		bool no_force_repaint;
	} state;
};
