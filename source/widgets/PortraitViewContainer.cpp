#include "widgets/PictureViewContainer.h"
#include "widgets/PictureView.h"
#include "widgets/KeepAspectRatioLayout.h"


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
	QGroupBox::resizeEvent(evt);
	if (pic->size() != view_port_size)
	{
		double zoom = (double)pic->size().width() / (double)view_port_size.width();
		pic->setZoom(zoom);
	}
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

#include <QWheelEvent>
#include <QResizeEvent>
#include <QApplication>

void PictureViewContainer::wheelEvent(QWheelEvent* evt)
{
	// qDebug() << evt->angleDelta();

	// this is a some what hacky way to scale the preview widget
	// by setting the layout's margin, which constraints the maximum size
	// that preview widget can have.
	int marign = 0;
	aspect_layout->getContentsMargins(&marign, nullptr, nullptr, nullptr);

	int step = evt->angleDelta().y() > 0 ? -5 : 5;
	marign = std::max(marign + step, 10);

	aspect_layout->setContentsMargins(marign, marign, marign, marign);

	// and here is another hack, by triggering resize event manually,
	// we are able to update the zoom of preview widget, forcing it
	// to update picture offset
	QApplication::postEvent(this, new QResizeEvent{ this->size(), this->size() });
}
