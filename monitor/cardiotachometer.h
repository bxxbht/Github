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

#ifndef CARDIOTACHOMETER_H
#define CARDIOTACHOMETER_H

#include <QFrame>
#include <QTimer>
#include "iconwidget.h"

QT_BEGIN_NAMESPACE
class Label;
QT_END_NAMESPACE

class HeartBeat : public IconWidget
{
	Q_OBJECT

public:
	HeartBeat(QWidget *parent = 0);
	virtual QSize minimumSizeHint() const;
	void setRate(int rate);

protected:

private Q_SLOTS:
	void heartBeat();

private:
	int palmus;
	int palmusCounter;
	bool on;
	QTimer *timer;
};


class Cardiotachometer : public QFrame
{
	Q_OBJECT

public:
	Cardiotachometer(QWidget *parent = 0);
	~Cardiotachometer();
	void setColor(const QColor &color);
	void setRate(const int rate);

protected:

private Q_SLOTS:

private:
	HeartBeat *icon;
	Label *identity;
	Label *meter;
};

#endif
