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

#include <QIcon>
#include <QPainter>
#include <QDebug>
#include "iconwidget.h"

IconWidget::IconWidget(QWidget *parent)
	: QWidget(parent)
{
	icon = new QIcon;
	init();
}

IconWidget::IconWidget(const QPixmap &pixmap, QWidget *parent)
{
	icon = new QIcon(pixmap);
	init();
}

IconWidget::IconWidget(const QIcon &other, QWidget *parent)
{
	icon = new QIcon(other);
	init();
}

IconWidget::IconWidget(const QString &fileName, QWidget *parent)
{
	icon = new QIcon(fileName);
	init();
}

IconWidget::~IconWidget()
{
	delete icon;
}

void IconWidget::addFile(const QString &fileName, const QSize &size, QIcon::Mode mode, QIcon::State state)
{
	icon->addFile(fileName, size, mode, state);
}

void IconWidget::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	icon->paint(&painter, rect(), align, mode, state);
}
