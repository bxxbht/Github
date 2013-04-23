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

#include "label.h"

Label::Label(QWidget *parent)
	: QLabel(parent)
{
	setAlignment(Qt::AlignCenter);
}

Label::Label(const char *text, QWidget *parent)
	: QLabel(text, parent)
{
	setAlignment(Qt::AlignCenter);
}

Label::~Label()
{
}

void Label::setBold(bool enable)
{
	QFont defaultFont(font());
	defaultFont.setBold(enable);
	setFont(defaultFont);
}

void Label::setPixelSize(int pixelSize)
{
	QFont defaultFont(font());
	defaultFont.setPixelSize(pixelSize);
	setFont(defaultFont);
}

void Label::setColor(const QColor &color)
{
	QPalette pal(palette());
	pal.setColor(QPalette::WindowText, color);
	setPalette(pal);
}

void Label::setLetterSpacing(QFont::SpacingType type, qreal spacing)
{
	QFont defaultFont(font());
	defaultFont.setLetterSpacing(type, spacing);
	setFont(defaultFont);
}
