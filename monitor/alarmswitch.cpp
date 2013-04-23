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


#include <QBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QDebug>
#include "label.h"
#include "alarmswitch.h"

AlarmIcon::AlarmIcon(QWidget *parent)
	: IconWidget(parent)
{
	addFile("images/alarm_normal_off.png", QSize(), QIcon::Normal, QIcon::Off);
	addFile("images/alarm_normal_on.png", QSize(), QIcon::Normal, QIcon::On);
	setState(QIcon::On);
}

QSize AlarmIcon::minimumSizeHint() const
{
	return QSize(32, 32);
}

AlarmSwitch::AlarmSwitch(QWidget *parent)
	: QFrame(parent)
{
	icon = new AlarmIcon(this);

	countDownLabel = new Label("--:--");
	countDownLabel->setPixelSize(24);
	countDownLabel->setColor(Qt::white);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(0);
	hLayout->addWidget(icon);
	hLayout->addWidget(countDownLabel);

	setLayout(hLayout);
}

AlarmSwitch::~AlarmSwitch()
{
}
