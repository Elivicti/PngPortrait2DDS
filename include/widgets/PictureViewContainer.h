#pragma once

#include <QGroupBox>


class PictureView;
class KeepAspectRatioLayout;

class PictureViewContainer : public QGroupBox
{
	Q_OBJECT;
public:
	PictureViewContainer(QWidget* parent = nullptr);

	void setPreviewWidget(PictureView* w);

	void setViewSize(const QSize& size);
	QSize viewSize() const { return view_port_size; }

Q_SIGNALS:
	void offsetChanged(QPoint offset);
	void scaleChanged(double scale);

protected:
	void resizeEvent(QResizeEvent* evt) override;
	void wheelEvent(QWheelEvent* evt) override;

private:
	PictureView* pic;
	KeepAspectRatioLayout* aspect_layout;

	QSize view_port_size;
};
