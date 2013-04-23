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

#include <QPainter>
#include <QTimer>
#include "alertbox.h"

AlertBox::AlertBox(QWidget *parent)
	: QLabel(parent)
{
	setAlignment(Qt::AlignJustify | Qt::AlignCenter);
}

AlertBox::AlertBox(const char *text, QWidget *parent)
	: QLabel(text, parent)
{
	setAlignment(Qt::AlignJustify | Qt::AlignCenter);
	QFont alerBoxFont(font());
	alerBoxFont.setBold(true);
	alerBoxFont.setPixelSize(16);
	setFont(alerBoxFont);

	defaultItem.level = Informational;
	defaultItem.message = text;
	defaultItem.expire = -1;

#if 0
	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(refresh()));
	connect(timer, SIGNAL(timeout()), this, SLOT(blanking()));
	timer->start(refleshRate);
#endif

	refleshCounter = 0;
	labelBlanking = false;
	labelBlankingCounter = 0;
}

AlertBox::~AlertBox()
{
}

void AlertBox::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);

	QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
#if 0
	gradient.setColorAt(0.0, Qt::white);
	gradient.setColorAt(0.2, Qt::green);
	// gradient.setColorAt(0.2, Qt::red);
	// gradient.setColorAt(0.2, Qt::yellow);
	gradient.setColorAt(1.0, Qt::black);
#else
	gradient.setColorAt(0.0, Qt::white);
	// gradient.setColorAt(0.0, QColor(Qt::green).lighter(300));
	gradient.setColorAt(0.2, QColor(Qt::green));
	gradient.setColorAt(1.0, QColor(Qt::green).darker(300));
#endif

	painter.fillRect(rect(), QBrush(gradient));

	if (!labelBlanking)
		QLabel::paintEvent(event);
}

void AlertBox::refresh()
{
	AlertItem *item;
	effectiveItem = &defaultItem;

	if (!criticalList.isEmpty()) {
		if (++refleshCounter > 6) {
			initCounter();
			update();
		}

		return;
	}

	if (!warningList.isEmpty()) {
		if (++refleshCounter > 16) {
			initCounter();
			update();
		}

		return;
	}

	if (!noticeList.isEmpty()) {
		if (++refleshCounter > 30) {
			initCounter();
			update();
		}

		return;
	}
}

void AlertBox::blanking()
{
	if (effectiveItem) {
		const AlertLevel &level = effectiveItem->level;
		int blankExpire;
		if (level == Notice) {
			blankExpire = 15;
		} else if (level == Warning) {
			blankExpire = 8;
		} else if (level == Critical) {
			blankExpire = 3;
		} else { // Informational
			blankExpire = -1;
		}
		if ((level != Informational) && (++labelBlankingCounter > blankExpire)) {
			labelBlankingCounter = 0;
			labelBlanking = !labelBlanking;
			update();
		}
	} else {
		initCounter();
	}
}

void AlertBox::initCounter()
{
	refleshCounter = 0;
	labelBlanking = false;
	labelBlankingCounter = 0;
}
