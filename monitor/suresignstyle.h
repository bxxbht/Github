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

#ifndef SURESIGNSTYLE_H
#define SURESIGNSTYLE_H

#include <QPlastiqueStyle>
#include <QPalette>

QT_BEGIN_NAMESPACE
class QPainterPath;
class QStyleOptionTabV3;
QT_END_NAMESPACE

class SuresignStyle : public QPlastiqueStyle
{
	Q_OBJECT

public:
	SuresignStyle() {}
	void polish(QPalette &palette);
	void polish(QWidget *widget);
	void unpolish(QWidget *widget);
	void drawControl(ControlElement element, const QStyleOption *option,
			QPainter *painter, const QWidget *widget) const;
	void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
			QPainter *painter, const QWidget *widget) const;
	virtual int pixelMetric(PixelMetric metric, const QStyleOption *option = 0, const QWidget *widget = 0) const;
	QRect subControlRect(ComplexControl control, const QStyleOptionComplex *option,
			SubControl subControl, const QWidget *widget) const;

private:
	void tabLayout(const QStyleOptionTabV3 *opt, const QWidget *widget,
			QRect *textRect, QRect *iconRect) const;
};

#endif
