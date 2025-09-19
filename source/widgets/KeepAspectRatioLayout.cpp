#include "widgets/KeepAspectRatioLayout.h"
#include <QWidget>
#include <QStyle>
#include <QMenuBar>

KeepAspectRatioLayout::KeepAspectRatioLayout(QWidget *parent)
	: QGridLayout{ parent }
	, aspect_ratio{ 1.0 }
{
	this->setAlignment(Qt::AlignCenter);
}

void KeepAspectRatioLayout::setGeometry(const QRect &rect)
{
	QLayout::setGeometry(rect);

	int rows = rowCount();
	int cols = columnCount();

	if (rows == 0 || cols == 0)
		return;

	QRect effective_rect = rect;

	if (QWidget* parent_widget = parentWidget())
	{
		QMenuBar* menu = parent_widget->findChild<QMenuBar*>();
		if (menu && menu->isVisible())
		{
			effective_rect.adjust(0, menu->height(), 0, 0);
		}
	}

	int left, top, right, bottom;
	getContentsMargins(&left, &top, &right, &bottom);
	effective_rect = rect.adjusted(left, top, -right, -bottom);

	int h_spacing = horizontalSpacing();
	int v_spacing = verticalSpacing();

	qreal layout_aspect_ratio = ((qreal)cols * aspect_ratio) / rows;
	qreal available_aspect_ratio = ((qreal)effective_rect.width()) / effective_rect.height();

	qreal layout_width  = 0.0;
	qreal layout_height = 0.0;

	if (available_aspect_ratio > layout_aspect_ratio)
	{
		layout_height = effective_rect.height();
		layout_width = layout_aspect_ratio * layout_height;
	}
	else
	{
		layout_width = effective_rect.width();
		layout_height = layout_width / layout_aspect_ratio;
	}

	Qt::Alignment align = alignment();
	if (align == 0)
		align = Qt::AlignCenter;
	QRect layout_rect{ QStyle::alignedRect(
		Qt::LayoutDirectionAuto,
		align,
		QSize{ (int)layout_width, (int)layout_height },
		effective_rect
	) };
	int start_x = layout_rect.x();
	int start_y = layout_rect.y();

	qreal w = 0.0;
	qreal h = 0.0;
	if (available_aspect_ratio > layout_aspect_ratio)
	{
		h = (layout_height - (rows - 1) * v_spacing) / rows;
		w = aspect_ratio * h;
	}
	else
	{
		w = (layout_width - (cols - 1) * h_spacing) / cols;
		h = w / aspect_ratio;
	}

	for (int i = 0; i < count(); ++i)
	{
		QLayoutItem *item = itemAt(i);
		if (!item)
			continue;

		int row, col, row_span, col_span;
		getItemPosition(i, &row, &col, &row_span, &col_span);

		// do not handle span atm
		if (row_span != 1 || col_span != 1)
			continue;

		int x = start_x + col * (w + h_spacing);
		int y = start_y + row * (h + v_spacing);

		QRect cell_rect{ x, y, (int)w, (int)h };

		item->setGeometry(cell_rect);

		// if item widget has a maximum size or minimum size
		// then the topleft point need to be adjusted
		if (QWidget* w = item->widget(); w->size() != cell_rect.size())
		{
			QSize target_size = w->size();
			QPoint delta{
				(cell_rect.width() - target_size.width()) / 2,
				(cell_rect.height() - target_size.height()) / 2
			};

			item->setGeometry(QStyle::alignedRect(
				Qt::LayoutDirectionAuto,
				this->alignment(),
				target_size,
				cell_rect
			));
		}
	}
}
