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
#include "ecgchannel.h"

EcgChannel::EcgChannel(QWidget *parent)
	: CurvePlotter(parent)
{
	init();
}

EcgChannel::EcgChannel(const char *text, QWidget *parent)
	: CurvePlotter(parent)
{
	identity.setText(text);
	init();
}

EcgChannel::~EcgChannel()
{

}

void EcgChannel::setIdentity(const char *str)
{
	identity.setText(str);
}

void EcgChannel::setLead(const char *str)
{
	lead.setText(str);
}

void EcgChannel::setGain(const char *str)
{
	gain.setText(str);
}

void EcgChannel::setSpeed(const char *str)
{
	speed.setText(str);

}

void EcgChannel::setMode(const char *str)
{
	mode.setText(str);
}

void EcgChannel::setColor(const QColor &color)
{
	setCurveColor(color);
	identity.setColor(color);
	lead.setColor(color);
	gain.setColor(color);
	speed.setColor(color);
	mode.setColor(color);
}

void EcgChannel::init()
{
	identity.setBold(true);
	QHBoxLayout *hLayout = new QHBoxLayout;
	// hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->setSpacing(16);

	hLayout->addWidget(&identity);
	hLayout->addWidget(&lead);
	hLayout->addWidget(&gain);
	hLayout->addStretch();
	hLayout->addWidget(&speed);
	hLayout->addWidget(&mode);

	QVBoxLayout *vLayout = new QVBoxLayout;
	// vLayout->setContentsMargins(0, 0, 0, 0);
	// vLayout->setSpacing(0);

	vLayout->addLayout(hLayout);
	vLayout->addStretch();

	setLayout(vLayout);
}
