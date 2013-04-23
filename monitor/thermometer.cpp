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

#include <QGridLayout>
#include <QLabel>
#include <QPainter>
#include <QDebug>

#include "label.h"
#include "thermometer.h"

Thermometer::Thermometer(QWidget *parent)
	: QFrame(parent)
{
	identity0 = new Label("TEMP1(C)");
	identity0->setPixelSize(16);
	identity0->setBold(true);

	identity1 = new Label("TEMP2(C)");
	identity1->setPixelSize(16);
	identity1->setBold(true);

	deltaT = new Label("delta T");

	tempLabel0 = new Label("-");
	tempLabel0->setPixelSize(48);
	tempLabel0->setBold(true);

	tempLabel1 = new Label("-");
	tempLabel1->setPixelSize(48);
	tempLabel1->setBold(true);

	QGridLayout *layout = new QGridLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(8);
	layout->addWidget(identity0, 0, 0, 1, 1);
	layout->addWidget(identity1, 0, 2, 1, 1);
	layout->addWidget(deltaT, 1, 1, 1, 1);
	layout->addWidget(tempLabel0, 1, 0, 1, 1);
	layout->addWidget(tempLabel1, 1, 2, 1, 1);

	setLayout(layout);
}

Thermometer::~Thermometer()
{
}

void Thermometer::setColor(const QColor &color)
{
	identity0->setColor(color);
	identity1->setColor(color);
	deltaT->setColor(color);
	tempLabel0->setColor(color);
	tempLabel1->setColor(color);
}

void Thermometer::setTemperature(ThermometerNo no, qreal temp)
{

}
