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

#ifndef THERMOMETER_H
#define THERMOMETER_H

#include <QFrame>
#include <QTimer>
#include "iconwidget.h"

QT_BEGIN_NAMESPACE
class Label;
QT_END_NAMESPACE

class Thermometer : public QFrame
{
	Q_OBJECT

	enum ThermometerNo {
		No0,
		No1,
	};

public:
	Thermometer(QWidget *parent = 0);
	~Thermometer();
	void setColor(const QColor &color);
	void setTemperature(ThermometerNo no, qreal temp);
	inline void setTemperatureNo0(qreal temp)
	{
		setTemperature(Thermometer::No0, temp);
	}
	inline void setTemperatureNo1(qreal temp)
	{
		setTemperature(Thermometer::No1, temp);
	}

protected:

private Q_SLOTS:

private:
	Label *identity0;
	Label *identity1;
	Label *deltaT;
	Label *tempLabel0;
	Label *tempLabel1;
};

#endif
