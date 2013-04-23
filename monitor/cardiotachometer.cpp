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
#include "cardiotachometer.h"

HeartBeat::HeartBeat(QWidget *parent)
	: IconWidget(parent)
{
	addFile("images/heart.png", QSize(), QIcon::Normal, QIcon::Off);
	addFile("images/heart-min.png", QSize(), QIcon::Normal, QIcon::On);

	palmus = 0;
	palmusCounter = 0;
	on = false;

	timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(heartBeat()));
}

QSize HeartBeat::minimumSizeHint() const
{
	return QSize(16, 16);
}

void HeartBeat::setRate(const int rate)
{
	if (rate <= 0)
		timer->stop();
	else {
		timer->start(50);
		palmus = 60 * 1000 / rate / 50 / 2;
	}
	palmusCounter = 0;
	on = false;
}

void HeartBeat::heartBeat()
{
	if (++palmusCounter > palmus) {
		palmusCounter = 0;
		on = !on;
		setState(on ? QIcon::On : QIcon::Off);
		update();
	}
}

Cardiotachometer::Cardiotachometer(QWidget *parent)
	: QFrame(parent)
{
	// setFrameStyle(QFrame::Box);
	icon = new HeartBeat(this);

	identity = new Label("HR");
	identity->setPixelSize(16);
	identity->setBold(true);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(8);
	hLayout->addWidget(icon);
	hLayout->addWidget(identity);
	hLayout->addStretch();

	meter = new Label("-");
	meter->setPixelSize(56);
	meter->setBold(true);

	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(0);
	vLayout->addLayout(hLayout);
	vLayout->addWidget(meter);

	setLayout(vLayout);

	setFixedWidth(128);
}

Cardiotachometer::~Cardiotachometer()
{
}

void Cardiotachometer::setRate(int rate)
{
	icon->setRate(rate);
	if (rate < 0)
		meter->setText("-");
	else
		meter->setText(QString::number(rate));
}

void Cardiotachometer::setColor(const QColor &color)
{
	identity->setColor(color);
	meter->setColor(color);
}
