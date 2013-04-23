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

#include "digitalclock.h"

DigitalClock::DigitalClock(QWidget *parent)
	: QLCDNumber(parent)
{
	setFixedWidth(220);
	setFrameStyle(QFrame::NoFrame);
	setSegmentStyle(Filled);
	// setSegmentStyle(Flat);
	setDigitCount(19);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
	timer->start(1000);

	showTime();
}

void DigitalClock::showTime()
{
	QDate date = QDate::currentDate();
	QTime time = QTime::currentTime();
	QString text = date.toString("yyyy-MM-dd");
	text += " ";
	text += time.toString("hh:mm:ss");
	if ((time.second() % 2) == 0) {
		text[13] = ' ';
		text[16] = ' ';
	}
	display(text);
}
