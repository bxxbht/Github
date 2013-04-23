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

#include <QHBoxLayout>
#include <QLabel>
#include "alertbox.h"
#include "alarmbar.h"

AlarmBar::AlarmBar(QWidget *parent)
	: QFrame(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	// setFixedHeight(40);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);
#if 0
	QLabel *physioAlarm = new QLabel("Physio Alarm");
	physioAlarm->setAlignment(Qt::AlignJustify | Qt::AlignCenter);
	QLabel *deviceAlarm = new QLabel("Device Alarm");
	deviceAlarm->setAlignment(Qt::AlignJustify | Qt::AlignCenter);
#else
	AlertBox *physioAlarm = new AlertBox("Physio Alarm");
	AlertBox *deviceAlarm = new AlertBox("Device Alarm");
#endif
	layout->addWidget(physioAlarm);
	layout->addWidget(deviceAlarm);
	setLayout(layout);
}

AlarmBar::~AlarmBar()
{
}

QSize AlarmBar::sizeHint() const
{
	return QSize(550, 40);
}
