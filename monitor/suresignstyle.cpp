/*
 * A simple patient monitor demo program.
 *
 * Copyright (C) 2011 longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <QtGui>
#include "suresignstyle.h"


void SuresignStyle::tabLayout(const QStyleOptionTabV3 *opt, const QWidget *widget, QRect *textRect, QRect *iconRect) const
{
	QRect tr = opt->rect;
	bool verticalTabs = opt->shape == QTabBar::RoundedEast
		|| opt->shape == QTabBar::RoundedWest
		|| opt->shape == QTabBar::TriangularEast
		|| opt->shape == QTabBar::TriangularWest;
	if (verticalTabs)
		tr.setRect(0, 0, tr.height(), tr.width()); //0, 0 as we will have a translate transform

	int verticalShift = pixelMetric(QStyle::PM_TabBarTabShiftVertical, opt, widget);
	int horizontalShift = pixelMetric(QStyle::PM_TabBarTabShiftHorizontal, opt, widget);
	int hpadding = pixelMetric(QStyle::PM_TabBarTabHSpace, opt, widget) / 2;
	int vpadding = pixelMetric(QStyle::PM_TabBarTabVSpace, opt, widget) / 2;
	if (opt->shape == QTabBar::RoundedSouth || opt->shape == QTabBar::TriangularSouth)
		verticalShift = -verticalShift;
	tr.adjust(hpadding, verticalShift - vpadding, horizontalShift - hpadding, vpadding);
	bool selected = opt->state & QStyle::State_Selected;
	if (selected) {
		tr.setTop(tr.top() - verticalShift);
		tr.setRight(tr.right() - horizontalShift);
	}

	// left widget
	if (!opt->leftButtonSize.isEmpty()) {
		tr.setLeft(tr.left() + 4 +
			(verticalTabs ? opt->leftButtonSize.height() : opt->leftButtonSize.width()));
	}
	// right widget
	if (!opt->rightButtonSize.isEmpty()) {
		tr.setRight(tr.right() - 4 -
		(verticalTabs ? opt->rightButtonSize.height() : opt->rightButtonSize.width()));
	}

	// icon
	if (!opt->icon.isNull()) {
		QSize iconSize = opt->iconSize;
		if (!iconSize.isValid()) {
			int iconExtent = pixelMetric(QStyle::PM_SmallIconSize);
			iconSize = QSize(iconExtent, iconExtent);
		}
		QSize tabIconSize = opt->icon.actualSize(iconSize,
						(opt->state & QStyle::State_Enabled) ? QIcon::Normal : QIcon::Disabled,
						(opt->state & QStyle::State_Selected) ? QIcon::On : QIcon::Off  );

		*iconRect = QRect(tr.left(), tr.center().y() - tabIconSize.height() / 2,
					tabIconSize.width(), tabIconSize .height());
		if (!verticalTabs)
			*iconRect = visualRect(opt->direction, opt->rect, *iconRect);
		tr.setLeft(tr.left() + tabIconSize.width() + 4);
	}

	if (!verticalTabs)
		tr = visualRect(opt->direction, opt->rect, tr);

	*textRect = tr;
}

void SuresignStyle::polish(QPalette &palette)
{
	// palette = QPalette(QColor("#484848"));

	QLinearGradient highlightGradient;
	highlightGradient.setColorAt(0.0, QColor("#ffe2a3"));
	highlightGradient.setColorAt(1.0, QColor("#ffc258"));

	palette = QApplication::palette();
	palette.setBrush(QPalette::Window, QColor("#1c1a19"));
	palette.setColor(QPalette::WindowText, QColor("#ffffff"));
	palette.setColor(QPalette::Text, QColor("#ffffff"));
	palette.setColor(QPalette::Base, QColor("#2b2a27"));
	palette.setColor(QPalette::AlternateBase, QColor("#1f1e1b"));
	palette.setBrush(QPalette::Highlight, highlightGradient);
	palette.setColor(QPalette::HighlightedText, QColor("#000000"));
	palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor("#808080"));
	palette.setColor(QPalette::Active, QPalette::ButtonText, QColor("#ffffff"));
	palette.setColor(QPalette::Inactive, QPalette::ButtonText, QColor("#000000"));

	// QPlastiqueStyle::polish(palette);
}

void SuresignStyle::polish(QWidget *widget)
{
	QPlastiqueStyle::polish(widget);
}

void SuresignStyle::unpolish(QWidget *widget)
{
	QPlastiqueStyle::unpolish(widget);
}

void SuresignStyle::drawControl(ControlElement element, const QStyleOption *option,
		QPainter *painter, const QWidget *widget) const
{
	switch (element) {
#if 0
	case CE_Header:
		if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
			QRegion clipRegion = painter->clipRegion();
			painter->setClipRect(opt->rect);
			drawControl(CE_HeaderSection, header, painter, widget);
			QStyleOptionHeader subopt = *header;
			subopt.rect = subElementRect(SE_HeaderLabel, header, widget);
			if (subopt.rect.isValid())
				proxy()->drawControl(CE_HeaderLabel, &subopt, p, widget);
			if (header->sortIndicator != QStyleOptionHeader::None) {
				subopt.rect = subElementRect(SE_HeaderArrow, opt, widget);
				drawPrimitive(PE_IndicatorHeaderArrow, &subopt, p, widget);
			}
			painter->setClipRegion(clipRegion);
		}
		break;
#endif
	case CE_HeaderSection:
		if (const QStyleOptionHeader *header = qstyleoption_cast<const QStyleOptionHeader *>(option)) {
			QStyleOptionHeader subopt = *header;
			QStyleOptionHeader::SectionPosition position = subopt.position;
			QRect rect = QRect(option->rect);

			painter->save();

			painter->setPen("#62625f");
			painter->drawLine(rect.topLeft(), rect.topRight());
			painter->setPen("#181715");
			painter->drawLine(rect.bottomLeft(), rect.bottomRight());

			if (position == QStyleOptionHeader::Beginning) {
				painter->setPen("#141414");
				painter->drawLine(rect.topRight(), rect.bottomRight());
				rect.adjust(0, 1, -1, -1);
				QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
				gradient.setColorAt(0.0, QColor("#403f3c"));
				gradient.setColorAt(1.0, QColor("#2b2a27"));
				painter->fillRect(rect, QBrush(gradient));
			} else if (position == QStyleOptionHeader::End) {
				painter->setPen("#545454");
				painter->drawLine(rect.topLeft(), rect.bottomLeft());
				rect.adjust(1, 1, 0, -1);
				QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
				gradient.setColorAt(0.0, QColor("#403f3c"));
				gradient.setColorAt(1.0, QColor("#2b2a27"));
				painter->fillRect(rect, QBrush(gradient));
			} else { /* QStyleOptionHeader::OnlyOneSection or  QStyleOptionHeader::Middle */
				painter->setPen("#141414");
				painter->drawLine(rect.topRight(), rect.bottomRight());
				painter->setPen("#545454");
				painter->drawLine(rect.topLeft(), rect.bottomLeft());
				rect.adjust(1, 1, -1, -1);
				QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
				gradient.setColorAt(0.0, QColor("#403f3c"));
				gradient.setColorAt(1.0, QColor("#2b2a27"));
				painter->fillRect(rect, QBrush(gradient));
			}

			painter->restore();
		}
		break;
	case CE_PushButton:
        	if (const QStyleOptionButton *btn = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			bool enabled = btn->state & State_Enabled;
			bool sunken = (btn->state & State_Sunken) || (btn->state & State_On);
			bool focus = btn->state & State_HasFocus;

			drawControl(CE_PushButtonBevel, btn, painter, widget);
			QStyleOptionButton subopt = *btn;
			subopt.rect = subElementRect(SE_PushButtonContents, btn, widget);

			if (enabled) {
				if (sunken || focus)
					subopt.palette.setColor(QPalette::ButtonText, QColor("#000000"));
				else
					subopt.palette.setColor(QPalette::ButtonText, QColor("#ffffff"));
			} else {
				subopt.palette.setColor(QPalette::ButtonText, QColor("#808080"));
			}

			drawControl(CE_PushButtonLabel, &subopt, painter, widget);
			if (btn->state & State_HasFocus) {
				QStyleOptionFocusRect fropt;
				fropt.QStyleOption::operator=(*btn);
				fropt.rect = subElementRect(SE_PushButtonFocusRect, btn, widget);
				drawPrimitive(PE_FrameFocusRect, &fropt, painter, widget);
			}
		}
		break;
	case CE_ScrollBarAddLine:
	case CE_ScrollBarSubLine:
	case CE_ScrollBarAddPage:
	case CE_ScrollBarSubPage:
		if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
			bool horizontal = scrollBar->orientation == Qt::Horizontal;
			QRect rect = QRect(option->rect);

			painter->save();

			if (horizontal) {
				QLinearGradient gradient(rect.topLeft(), rect.bottomLeft());
				gradient.setColorAt(0.0, QColor("#2a2926"));
				gradient.setColorAt(1.0, QColor("#41403d"));
				painter->fillRect(rect, QBrush(gradient));
				painter->setBrushOrigin((element == CE_ScrollBarAddPage) ? rect.width() : 1, 0);
				painter->fillRect(QRect(rect.topLeft(), QSize(rect.width(), 1)),
						QBrush(QColor("#252421")));
				painter->fillRect(QRect(rect.bottomLeft(), QSize(rect.width(), 1)),
						QBrush(QColor("#4d4c49")));
			} else {
				QLinearGradient gradient(rect.topLeft(), rect.topRight());
				gradient.setColorAt(0.0, QColor("#2a2926"));
				gradient.setColorAt(1.0, QColor("#41403d"));
				painter->fillRect(rect, QBrush(gradient));
				painter->setBrushOrigin(0, (element == CE_ScrollBarAddPage) ? rect.height() : 1);
				painter->fillRect(QRect(rect.topLeft(), QSize(1, rect.height())),
						QBrush(QColor("#252421")));
				painter->fillRect(QRect(rect.topRight(), QSize(1, rect.height())),
						QBrush(QColor("#4d4c49")));
			}

			painter->restore();

			if ((element == CE_ScrollBarAddLine) || (element == CE_ScrollBarSubLine)) {
				PrimitiveElement arrow;
				if (horizontal) {
					if (element == CE_ScrollBarAddLine)
						arrow = option->direction == Qt::LeftToRight ? PE_IndicatorArrowRight : PE_IndicatorArrowLeft;
					else
						arrow = option->direction == Qt::LeftToRight ? PE_IndicatorArrowLeft : PE_IndicatorArrowRight;
				} else {
					if (element == CE_ScrollBarAddLine)
						arrow = PE_IndicatorArrowDown;
					else
						arrow = PE_IndicatorArrowUp;
				}
        			QStyleOption arrowOpt = *option;
        			arrowOpt.rect = option->rect.adjusted(2, 1, -2, -1);
        			drawPrimitive(arrow, &arrowOpt, painter, widget);
			}
		}
		break;
	case CE_ScrollBarSlider:
		if (const QStyleOptionSlider *scrollBar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
			bool horizontal = scrollBar->orientation == Qt::Horizontal;
			bool isEnabled = scrollBar->state & State_Enabled;

			QRect rect(option->rect);
			QRectF rectF(rect);

			painter->save();

			if (horizontal) {
				rectF.adjust(0.5, 1.5, -0.5, -1.5);
				painter->fillRect(QRect(rect.topLeft(), QSize(rect.width(), 1)),
						QBrush(QColor("#252421")));
				painter->fillRect(QRect(rect.bottomLeft(), QSize(rect.width(), 1)),
						QBrush(QColor("#4d4c49")));
			} else {
				rectF.adjust(0.5, 0.5, -1.5, -0.5);
				painter->fillRect(QRect(rect.topLeft(), QSize(1, rect.height())),
						QBrush(QColor("#252421")));
				painter->fillRect(QRect(rect.topRight(), QSize(1, rect.height())),
						QBrush(QColor("#4d4c49")));
			}

			painter->setRenderHint(QPainter::Antialiasing, true);

			QColor borderColor("#2c2b2a");
			QColor gradientColorStart("#62615c");
			QColor gradientColorStop("#474541");
			QColor highlightEdgeColor("#71716f");

			painter->setBrush(QBrush(Qt::NoBrush));
			painter->setPen(QPen(borderColor, 1));
			painter->drawRoundedRect(rectF, 3, 3);

			QRectF fillRect = QRectF(rectF.topLeft() + QPointF(0.5, 0.5),
					QSizeF(rectF.width() - 1.0, rectF.height() - 1.0));
			QRectF outlineRect = QRectF(rectF.topLeft() + QPointF(1.0, 1.0),
					QSizeF(rectF.width() - 2.0, rectF.height() - 2.0));

			if (horizontal) {
				QLinearGradient gradient(fillRect.topLeft() + QPointF(0, 2), fillRect.bottomLeft());
				gradient.setColorAt(0.0, gradientColorStart);
				gradient.setColorAt(1.0, gradientColorStop);
				painter->setBrush(QBrush(gradient));
				painter->setPen(QPen(Qt::NoPen));
				painter->drawRoundedRect(fillRect, 2, 2);

				QRectF topRectClip = QRectF(rectF.topLeft(), QSizeF(rectF.width(), 2.0));
				painter->setClipRect(topRectClip);
				painter->setBrush(QBrush(Qt::NoBrush));
				painter->setPen(QPen(highlightEdgeColor, 1));
				painter->drawRoundedRect(outlineRect, 2, 2);
			} else {
				QLinearGradient gradient(fillRect.topLeft() + QPointF(2, 0), fillRect.topRight());
				gradient.setColorAt(0.0, gradientColorStart);
				gradient.setColorAt(1.0, gradientColorStop);
				painter->setBrush(QBrush(gradient));
				painter->setPen(QPen(Qt::NoPen));
				painter->drawRoundedRect(fillRect, 2, 2);

				QRectF leftRectClip = QRectF(rectF.topLeft(), QSizeF(2.0, rectF.height()));
				painter->setClipRect(leftRectClip);
				painter->setBrush(QBrush(Qt::NoBrush));
				painter->setPen(QPen(highlightEdgeColor, 1));
				painter->drawRoundedRect(outlineRect, 2, 2);
			}
			painter->restore();
		}
		break;
	case CE_TabBarTabShape:
		if (const QStyleOptionTab *tab = qstyleoption_cast<const QStyleOptionTab *>(option)) {
			if (tab->shape != QTabBar::RoundedNorth && tab->shape != QTabBar::RoundedWest &&
				tab->shape != QTabBar::RoundedSouth && tab->shape != QTabBar::RoundedEast) {
				QPlastiqueStyle::drawControl(element, option, painter, widget);
				break;
			}
			bool rtlHorTabs = (tab->direction == Qt::RightToLeft
					&& (tab->shape == QTabBar::RoundedNorth || tab->shape == QTabBar::RoundedSouth));
			bool selected = tab->state & State_Selected;
			bool lastTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::End)
					|| (rtlHorTabs && tab->position == QStyleOptionTab::Beginning));
			bool firstTab = ((!rtlHorTabs && tab->position == QStyleOptionTab::Beginning)
					|| (rtlHorTabs && tab->position == QStyleOptionTab::End));
			bool onlyOne = tab->position == QStyleOptionTab::OnlyOneTab;
#if 0 /* {{{ */
			int borderThickness = proxy()->pixelMetric(PM_TabBarBaseOverlap, tab, widget);

			switch (tab->shape) {
			case QTabBar::RoundedNorth:
				if (!selected)
					rect.adjust(0, 0, 0, -borderThickness);
				break;
			case QTabBar::RoundedWest:
			case QTabBar::RoundedEast:
			case QTabBar::RoundedSouth:
				break ;
			default:
				break;
			}
#endif /* }}} */
			QColor borderColor("#000000");
			QColor gradientColorStart;
			QColor gradientColorStop;
			QColor topOutlineColor;
			if (selected) {
				gradientColorStart.setNamedColor("#2b2a27");
				gradientColorStop.setNamedColor("#2b2a27");
				topOutlineColor.setNamedColor("#656462");
			} else {
				gradientColorStart.setNamedColor("#4d4c49");
				gradientColorStop.setNamedColor("#3d3b39");
				topOutlineColor.setNamedColor("#767472");
			}

			QRectF rectF(tab->rect);
			QRect rectClip(tab->rect);
			if (onlyOne)
				rectF.adjust(0.5, 0.5, -0.5, -0.5);
			else {
				if (firstTab) {
					rectF.adjust(0.5, 0.5, -1.5, -0.5);
				} else if (lastTab) {
					rectF.adjust(1.5, 0.5, -0.5, -0.5);
				} else {
					rectF.adjust(1.5, 0.5, -1.5, -0.5);
				}
			}
			rectClip.adjust(0, 0, 0, -3);

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);
			if (!selected)
				painter->setClipRect(rectClip);

			painter->setBrush(QBrush(Qt::NoBrush));
			painter->setPen(QPen(borderColor, 1));
			painter->drawRoundedRect(rectF, 5, 5);

			QRectF fillRect = QRectF(rectF.topLeft() + QPointF(0.5, 0.5),
					QSizeF(rectF.width() - 1.0, rectF.height() - 1.0));
			QLinearGradient gradient(fillRect.topLeft() + QPointF(0, 2), fillRect.bottomLeft() + QPointF(0, -1));
			gradient.setColorAt(0.0, gradientColorStart);
			gradient.setColorAt(1.0, gradientColorStop);
			painter->setBrush(QBrush(gradient));
			painter->setPen(QPen(Qt::NoPen));
			painter->drawRoundedRect(fillRect, 4, 4);

			if (selected) {
				painter->setBrush(QBrush(gradientColorStop));
				painter->drawRect(QRectF(fillRect.bottomLeft() + QPointF(0, -3),
							fillRect.bottomRight() + QPointF(0, 1)));
			}

			QRectF outlineRect = QRectF(rectF.topLeft() + QPointF(1.0, 1.0),
					QSizeF(rectF.width() - 2.0, rectF.height() - 2.0));

			QRectF topRectClip = QRectF(rectF.topLeft(), QSizeF(rectF.width() - 1.0, 2.0));
			painter->setClipRect(topRectClip);
			painter->setBrush(QBrush(Qt::NoBrush));
			painter->setPen(QPen(topOutlineColor, 1));
			painter->drawRoundedRect(outlineRect, 4, 4);

			painter->restore();
		}
		break;
	default:
		QPlastiqueStyle::drawControl(element, option, painter, widget);
		break;
	}
}

void SuresignStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
		QPainter *painter, const QWidget *widget) const
{
	switch (element) {
	case PE_FrameFocusRect:
		break;
	case PE_FrameTabWidget:
		if (const QStyleOptionTabWidgetFrame *twf = qstyleoption_cast<const QStyleOptionTabWidgetFrame *>(option)) {
			int borderThickness = proxy()->pixelMetric(PM_TabBarBaseOverlap, twf, widget);
			bool reverse = (twf->direction == Qt::RightToLeft);

			painter->save();

			QRect tabBarRect;
			switch (twf->shape) {
			case QTabBar::RoundedNorth:
				if (reverse)
					tabBarRect = QRect(twf->rect.right() - twf->leftCornerWidgetSize.width() - twf->tabBarSize.width() + 1,
							twf->rect.top(), twf->tabBarSize.width(), borderThickness);
				else
					tabBarRect = QRect(twf->rect.left() + twf->leftCornerWidgetSize.width(), twf->rect.top(),
							twf->tabBarSize.width(), borderThickness);
				break;
			case QTabBar::RoundedWest:
				tabBarRect = QRect(twf->rect.left(), twf->rect.top() + twf->leftCornerWidgetSize.height(),
						borderThickness, twf->tabBarSize.height());
				break;
			case QTabBar::RoundedEast:
				tabBarRect = QRect(twf->rect.right() - borderThickness + 1, twf->rect.top()  + twf->leftCornerWidgetSize.height(),
						borderThickness, twf->tabBarSize.height());
				break;
			case QTabBar::RoundedSouth:
				if (reverse)
					tabBarRect = QRect(twf->rect.right() - twf->leftCornerWidgetSize.width() - twf->tabBarSize.width() + 1,
							twf->rect.bottom() - borderThickness + 1, twf->tabBarSize.width(), borderThickness);
				else
					tabBarRect = QRect(twf->rect.left() + twf->leftCornerWidgetSize.width(),
							twf->rect.bottom() - borderThickness + 1, twf->tabBarSize.width(), borderThickness);
				break;
			default:
				break;
			}

#if 0 /* {{{ */
			QRegion region(twf->rect);
			region -= tabBarRect;
			painter->setClipRegion(region);
#endif /* }}} */

			painter->setPen(QPen(QColor("#656462")));
			painter->drawLine(option->rect.topLeft(), option->rect.topRight());
			painter->fillRect(option->rect.adjusted(0, 1, 0, -1), QBrush(QColor("#2b2a27")));
			painter->setPen(QPen(QColor("#1f1e1c")));
			painter->drawLine(option->rect.bottomLeft(), option->rect.bottomRight());
			painter->restore();
		}

		break;
	case PE_PanelButtonCommand:
		if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			bool enabled = button->state & State_Enabled;
			bool sunken = (button->state & State_Sunken) || (button->state & State_On);
			bool focus = button->state & State_HasFocus;

			QColor borderColor("#000000");
			QColor gradientColorStart;
			QColor gradientColorStop;
			QColor topOutlineColor;
			QColor bottomOutlineColor;

			if (enabled) {
				if (focus) {
					if (sunken) {
						gradientColorStart.setNamedColor("#ffe0a0");
						gradientColorStop.setNamedColor("#ffc258");
						topOutlineColor.setNamedColor("#fff2d6");
						bottomOutlineColor.setNamedColor("#e08f35");
					} else {
						gradientColorStart.setNamedColor("#cbc5bc");
						gradientColorStop.setNamedColor("#a7a29a");
						topOutlineColor.setNamedColor("#e8e6e1");
						bottomOutlineColor.setNamedColor("#74706a");
					}
				} else {
					gradientColorStart.setNamedColor("#54524f");
					gradientColorStop.setNamedColor("#373534");
					topOutlineColor.setNamedColor("#8b8a88");
					bottomOutlineColor.setNamedColor("#2a2926");
				}
			} else {
				gradientColorStart.setNamedColor("#3f3e3a");
				gradientColorStop.setNamedColor("#3f3e3a");
				topOutlineColor.setNamedColor("#3f3e3a");
				bottomOutlineColor.setNamedColor("#3f3e3a");
			}
			painter->save();

			painter->setRenderHint(QPainter::Antialiasing, true);

			QRectF rectF = button->rect;
			rectF.adjust(0.5, 0.5, -0.5, -0.5);

			painter->setBrush(QBrush(Qt::NoBrush));
			painter->setPen(QPen(borderColor, 1));
			painter->drawRoundedRect(rectF, 5, 5);

			QRectF fillRect = QRectF(rectF.topLeft() + QPointF(0.5, 0.5),
					QSizeF(rectF.width() - 1.0, rectF.height() - 1.0));
			QLinearGradient gradient(fillRect.topLeft() + QPointF(0, 2), fillRect.bottomLeft() + QPointF(0, -1));
			gradient.setColorAt(0.0, gradientColorStart);
			gradient.setColorAt(1.0, gradientColorStop);
			painter->setBrush(QBrush(gradient));
			painter->setPen(QPen(Qt::NoPen));
			painter->drawRoundedRect(fillRect, 4, 4);

			QRectF outlineRect = QRectF(rectF.topLeft() + QPointF(1.0, 1.0),
					QSizeF(rectF.width() - 2.0, rectF.height() - 2.0));

			QRectF topRectClip = QRectF(rectF.topLeft(), QSizeF(rectF.width() - 1.0, 2.0));
			painter->setClipRect(topRectClip);
			painter->setBrush(QBrush(Qt::NoBrush));
			painter->setPen(QPen(topOutlineColor, 1));
			painter->drawRoundedRect(outlineRect, 4, 4);

			QRectF bottomRectClip = QRectF(rectF.bottomLeft() + QPointF(0.0, -3.0),
					QSizeF(rectF.width() - 1.0, 2.0));
			painter->setClipRect(bottomRectClip);
			painter->setBrush(Qt::NoBrush);
			painter->setPen(QPen(bottomOutlineColor, 1));
			painter->drawRoundedRect(outlineRect, 4, 4);

			painter->restore();
		}
		break;
	case PE_IndicatorCheckBox:
		if (const QStyleOptionButton *button = qstyleoption_cast<const QStyleOptionButton *>(option)) {
			bool enabled = button->state & State_Enabled;
			bool sunken = button->state & State_Sunken;
			bool focus = button->state & State_HasFocus;
			bool on = button->state & State_On;

			QRectF rectF = button->rect;
			rectF.adjust(0.5, 0.5, -0.5, -0.5);

			QColor penColor;
			QColor borderColor("#5b5a55");
			QColor fillColor;

			if (enabled) {
				penColor.setNamedColor("#000000");
				if (focus) {
					fillColor.setNamedColor("#ffffff");
				} else {
					fillColor.setNamedColor("#a8a49b");
				}
			} else {
				penColor.setNamedColor("#808080");
				fillColor.setNamedColor("#3f3e3a");
			}

			painter->save();
			painter->setRenderHint(QPainter::Antialiasing, true);

			painter->setBrush(QBrush(Qt::NoBrush));
			painter->setPen(QPen(borderColor, 1));
			painter->drawRect(rectF);

			QRectF fillRect = QRectF(rectF.topLeft() + QPointF(0.5, 0.5),
					QSizeF(rectF.width() - 1.0, rectF.height() - 1.0));
			painter->setBrush(fillColor);
			painter->setPen(QPen(Qt::NoPen));
			painter->drawRect(fillRect);

			if (on || (enabled && sunken)) {
				painter->setPen(QPen(penColor, 2));
				QRect rect(button->rect);
				const QLine lines[2] = {
					QLine(rect.left() + 3, rect.top() + 6, rect.left() + 5, rect.bottom() - 2),
					QLine(rect.left() + 5, rect.bottom() - 2, rect.right() - 2, rect.top() + 4)};
				painter->drawLines(lines, 2);
			}

			painter->restore();
		}
                break;
	case PE_PanelItemViewItem:
		if (const QStyleOptionViewItemV4 *vopt = qstyleoption_cast<const QStyleOptionViewItemV4 *>(option)) {
			QPalette::ColorGroup cg = (widget ? widget->isEnabled() : (vopt->state & QStyle::State_Enabled))
				? QPalette::Normal : QPalette::Disabled;
			if (cg == QPalette::Normal && !(vopt->state & QStyle::State_Active))
				cg = QPalette::Inactive;

			if (vopt->showDecorationSelected && (vopt->state & QStyle::State_Selected)) {
				QLinearGradient *gradient = (QLinearGradient *)vopt->palette.brush(cg, QPalette::Highlight).gradient();
				gradient->setStart(vopt->rect.topLeft());
				gradient->setFinalStop(vopt->rect.bottomLeft());
				painter->fillRect(vopt->rect, vopt->palette.brush(cg, QPalette::Highlight));
			} else {
				if (vopt->backgroundBrush.style() != Qt::NoBrush) {
					QPointF oldBO = painter->brushOrigin();
					painter->setBrushOrigin(vopt->rect.topLeft());
					painter->fillRect(vopt->rect, vopt->backgroundBrush);
					painter->setBrushOrigin(oldBO);
				}

				if (vopt->state & QStyle::State_Selected) {
					QRect textRect = subElementRect(QStyle::SE_ItemViewItemText, option, widget);
					painter->fillRect(textRect, vopt->palette.brush(cg, QPalette::Highlight));
				}
			}
		}
		break;
	default:
		QPlastiqueStyle::drawPrimitive(element, option, painter, widget);
		break;
	}
}

int SuresignStyle::pixelMetric(PixelMetric metric, const QStyleOption *option, const QWidget *widget) const
{
	int ret = -1;
	switch (metric) {
	case PM_ComboBoxFrameWidth:
		ret = 32;
		break;
	case PM_ScrollBarExtent:
		ret = 20;
		break;
	default:
		break;
	}

	return ret != -1 ? ret : QPlastiqueStyle::pixelMetric(metric, option, widget);
}

QRect SuresignStyle::subControlRect(ComplexControl control, const QStyleOptionComplex *option,
			SubControl subControl, const QWidget *widget) const
{
	QRect rect = QWindowsStyle::subControlRect(control, option, subControl, widget);

	switch (control) {
	case CC_ScrollBar:
		if (const QStyleOptionSlider *scrollbar = qstyleoption_cast<const QStyleOptionSlider *>(option)) {
			const QRect scrollBarRect = scrollbar->rect;
			int sbextent = pixelMetric(PM_ScrollBarExtent, scrollbar, widget);
			int maxlen = ((scrollbar->orientation == Qt::Horizontal) ?
					scrollBarRect.width() : scrollBarRect.height()) - (sbextent * 2);
			int sliderlen;

			// calculate slider length
			if (scrollbar->maximum != scrollbar->minimum) {
				uint range = scrollbar->maximum - scrollbar->minimum;
				sliderlen = (qint64(scrollbar->pageStep) * maxlen) / (range + scrollbar->pageStep);

				int slidermin = pixelMetric(PM_ScrollBarSliderMin, scrollbar, widget);
				if (sliderlen < slidermin || range > INT_MAX / 2)
					sliderlen = slidermin;
				if (sliderlen > maxlen)
					sliderlen = maxlen;
			} else {
				sliderlen = maxlen;
			}

			int sliderstart = sbextent + sliderPositionFromValue(scrollbar->minimum,
					scrollbar->maximum, scrollbar->sliderPosition,
					maxlen - sliderlen, scrollbar->upsideDown);

			switch (subControl) {
			case SC_ScrollBarSubLine:			// top/left button
				if (scrollbar->orientation == Qt::Horizontal) {
					int buttonWidth = qMin(scrollBarRect.width() / 2, sbextent);
					rect.setRect(0, 0, buttonWidth, scrollBarRect.height());
				} else {
					int buttonHeight = qMin(scrollBarRect.height() / 2, sbextent);
					rect.setRect(0, 0, scrollBarRect.width(), buttonHeight);
				}
				break;
			case SC_ScrollBarAddLine:			// bottom/right button
				if (scrollbar->orientation == Qt::Horizontal) {
					int buttonWidth = qMin(scrollBarRect.width()/2, sbextent);
					rect.setRect(scrollBarRect.width() - buttonWidth, 0, buttonWidth, scrollBarRect.height());
				} else {
					int buttonHeight = qMin(scrollBarRect.height()/2, sbextent);
					rect.setRect(0, scrollBarRect.height() - buttonHeight, scrollBarRect.width(), buttonHeight);
				}
				break;
			case SC_ScrollBarSubPage:			// between top/left button and slider
				if (scrollbar->orientation == Qt::Horizontal)
					rect.setRect(sbextent, 0, sliderstart - sbextent, scrollBarRect.height());
				else
					rect.setRect(0, sbextent, scrollBarRect.width(), sliderstart - sbextent);
				break;
			case SC_ScrollBarAddPage:			// between bottom/right button and slider
				if (scrollbar->orientation == Qt::Horizontal)
					rect.setRect(sliderstart + sliderlen, 0,
							maxlen - sliderstart - sliderlen + sbextent, scrollBarRect.height());
				else
					rect.setRect(0, sliderstart + sliderlen, scrollBarRect.width(),
							maxlen - sliderstart - sliderlen + sbextent);
				break;
			case SC_ScrollBarGroove:
				if (scrollbar->orientation == Qt::Horizontal)
					rect.setRect(sbextent, 0, scrollBarRect.width() - sbextent * 2,
							scrollBarRect.height());
				else
					rect.setRect(0, sbextent, scrollBarRect.width(),
							scrollBarRect.height() - sbextent * 2);
				break;
			case SC_ScrollBarSlider:
				if (scrollbar->orientation == Qt::Horizontal)
					rect.setRect(sliderstart, 0, sliderlen, scrollBarRect.height());
				else
					rect.setRect(0, sliderstart, scrollBarRect.width(), sliderlen);
				break;
			default:
				break;
			}
			rect = visualRect(scrollbar->direction, scrollBarRect, rect);
		}
		break;
	default:
		break;
	}

	return rect;
}
