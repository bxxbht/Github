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
#include <QPainter>

#include "alarmswitch.h"
#include "powerstatus.h"
#include "netstatus.h"
#include "statusbar.h"

StatusBar::StatusBar(QWidget *parent)
	: QFrame(parent)
{
	setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	// setFixedSize(250, 40);

	PowerStatus *powerStatus = new PowerStatus;
	NetStatus *netStatus = new NetStatus;
	AlarmSwitch *alarmSwitch = new AlarmSwitch;
	
	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	// layout->setSpacing(0);
	layout->insertSpacing(0, 8);
	layout->addWidget(powerStatus);
	layout->addWidget(netStatus);
	layout->addWidget(alarmSwitch);
	layout->addStretch();
	setLayout(layout);
}

StatusBar::~StatusBar()
{
}

QSize StatusBar::sizeHint() const
{
	return QSize(250, 40);
}

void StatusBar::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	painter.setPen(QPen(QColor("#3c3a39"), 1));
	painter.drawLine(rect().bottomLeft(), rect().bottomRight());

	QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
	gradient.setColorAt(0.0, QColor("#050404"));
	gradient.setColorAt(1.0, QColor("#2b2b2a"));
	painter.fillRect(rect().adjusted(0, 0, 0, -1), QBrush(gradient));
}
