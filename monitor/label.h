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

#ifndef LABEL_H
#define LABEL_H

#include <QLabel>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class Label : public QLabel
{
	Q_OBJECT

public:

	Label(QWidget *parent = 0);
	Label(const char *text, QWidget *parent = 0);
	~Label();
	void setBold(bool enable);
	void setPixelSize(int pixelSize);
	void setColor(const QColor &color);
	void setLetterSpacing(QFont::SpacingType type, qreal spacing);

protected:

private Q_SLOTS:

private:
};

#endif
