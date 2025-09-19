#include "widgets/PictureView.h"

#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>

PictureView::PictureView(QWidget* parent)
	: QFrame{ parent }
	, data{ QPixmap{}, QPoint{ 0, 0 }, 1.0, 1.0 }
	, drag_button{ Qt::NoButton }, wheel_step{ 0.0 }, cursor_as_whl_scale_center{ false }, track_mouse_drag{ false }
	, state{ false, false, QPoint{ 0, 0 }, QPoint{ 0, 0 }, false }
{
	connect(this, &PictureView::offsetChanged, this, &PictureView::force_repaint);
	connect(this, &PictureView::scaleChanged,  this, &PictureView::force_repaint);
}

PictureView::~PictureView() {}

void PictureView::paintEvent(QPaintEvent* evt)
{
	QPainter painter{ this };
	painter.setRenderHints(QPainter::SmoothPixmapTransform);

	auto& picture = data.picture;

	if (!picture.isNull())
	{
		QRect target{ data.offset * data.zoom_factor, picture.rect().size() * data.scale * data.zoom_factor };
		painter.drawPixmap(target, picture);
	}

	QFrame::paintEvent(evt);
}
void PictureView::mousePressEvent(QMouseEvent* evt)
{
	bool valid_mouse_press = (evt->button() == drag_button) && !data.picture.isNull();

	if (state.dragging && !valid_mouse_press)
	{
		if (track_mouse_drag)
			setOffset(state.old_offset);
		else
		{
			data.offset = state.old_offset;
			this->update();
		}
	}
	state.dragging = valid_mouse_press;

	if (!state.dragging)
		return;

	state.old_offset = data.offset;
	state.cursor_offset = data.offset - (evt->pos() / data.zoom_factor);
}
void PictureView::mouseMoveEvent(QMouseEvent* evt)
{
	if (!state.dragging)
		return;

	if (track_mouse_drag)
		setOffset((evt->pos() / data.zoom_factor) + state.cursor_offset);
	else
	{
		data.offset = (evt->pos() / data.zoom_factor) + state.cursor_offset;
		this->update();
	}
}
void PictureView::mouseReleaseEvent(QMouseEvent* evt)
{
	if (evt->button() != drag_button)
		return;
	if (state.dragging)
		Q_EMIT offsetChanged(data.offset, state.old_offset);
	state.dragging = false;
}

struct FlagGuard
{
	bool& flag;

	FlagGuard(bool& f, bool value)
		: flag{ f } { flag = value; }
	~FlagGuard() { flag = !flag; }
};

void PictureView::wheelEvent(QWheelEvent* evt)
{
	if (wheel_step <= 0.0 || state.dragging || data.picture.isNull())
		return;

	FlagGuard scaling_flag{ state.scaling, true };

	double old_scale = data.scale;
	state.no_force_repaint = cursor_as_whl_scale_center; // we only want to trigger repaint once
	setScale(data.scale + wheel_step * ((double)evt->angleDelta().y() / 120.0));

	if (cursor_as_whl_scale_center)
	{
		state.no_force_repaint = false;
		QPoint center = (evt->position() / data.zoom_factor).toPoint();
		QPoint center_offset = (data.offset - center) * data.scale / old_scale;

		setOffset(center + center_offset);
	}
}
