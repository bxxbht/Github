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
#include "cardiotachometer.h"
#include "thermometer.h"
#include "parameterpanel.h"

ParameterPanel::ParameterPanel(QWidget *parent)
	: QFrame(parent)
{
	Cardiotachometer *cardiotachometer = new Cardiotachometer;
	cardiotachometer->setRate(100);
	cardiotachometer->setColor(Qt::green);

	Thermometer *thermometer = new Thermometer;
	thermometer->setColor(Qt::red);

	QVBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(cardiotachometer);
	layout->addWidget(thermometer);
	layout->addStretch();

	setLayout(layout);
}

ParameterPanel::~ParameterPanel()
{
}
