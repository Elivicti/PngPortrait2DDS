#include "widgets/PictureViewContainer.h"
#include "widgets/PictureView.h"
#include "widgets/KeepAspectRatioLayout.h"

#include <QCoreApplication>
#include <QResizeEvent>
#include <QWheelEvent>

PictureViewContainer::PictureViewContainer(QWidget* parent)
	: QGroupBox{ parent }
	, pic{ nullptr }, view_port_size{}
	, aspect_layout{ new KeepAspectRatioLayout{ this } }
{
	aspect_layout->setSpacing(10);
	aspect_layout->setContentsMargins(20, 20, 20, 20);
}

void PictureViewContainer::resizeEvent(QResizeEvent* evt)
{
	double zoom = (double)pic->size().width() / (double)view_port_size.width();
	pic->setZoom(zoom);

	// prevent margin is too big or too small after resizing the window
	QMargins margin{ aspect_layout->contentsMargins() };
	double resize_factor = (double)evt->size().width() / (double)evt->oldSize().width();
	setPreviewWidgetZoomViaLayoutMargin(margin.left() * resize_factor, margin.top() * resize_factor);

	QGroupBox::resizeEvent(evt);
}

void PictureViewContainer::setPreviewWidget(PictureView* w)
{
	pic = w;
	aspect_layout->addWidget(pic);
}

void PictureViewContainer::setViewSize(const QSize& size)
{
	view_port_size = size;
	aspect_layout->setAspectRatio(view_port_size);
	// pic->setMaximumSize(view_port_size);
}

void PictureViewContainer::wheelEvent(QWheelEvent* evt)
{
	// this is a some what hacky way to scale the preview widget
	// by setting the layout's margin, which constraints the maximum size
	// that preview widget can have.
	QMargins margin{ aspect_layout->contentsMargins() };

	int step = evt->angleDelta().y() > 0 ? -5 : 5;
	setPreviewWidgetZoomViaLayoutMargin(margin.left() + step, margin.top() + step);

	// and here is another hack, by triggering resize event manually,
	// we are able to update the zoom of preview widget, forcing it
	// to update picture offset
	QCoreApplication::postEvent(this, new QResizeEvent{ this->size(), this->size() });
}

void PictureViewContainer::setPreviewWidgetZoomViaLayoutMargin(int margin_horizontal, int margin_vertical)
{
	int min_h = 50;
	int min_w = min_h * aspect_layout->aspectRatio();

	margin_horizontal = std::clamp(margin_horizontal, 10, (this->width()  - min_w) / 2);
	margin_vertical   = std::clamp(margin_vertical,   10, (this->height() - min_h) / 2);

	aspect_layout->setContentsMargins(margin_horizontal, margin_vertical, margin_horizontal, margin_vertical);
}
