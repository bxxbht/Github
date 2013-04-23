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

#ifndef ECG_CHANNEL_H
#define ECG_CHANNEL_H

#include "curveplotter.h"
#include "label.h"

QT_BEGIN_NAMESPACE
class QString;
QT_END_NAMESPACE

class EcgChannel : public CurvePlotter
{
	Q_OBJECT

public:
	EcgChannel(QWidget *parent = 0);
	EcgChannel(const char *text, QWidget *parent = 0);
	~EcgChannel();
	void setIdentity(const char *str);
	void setLead(const char *str);
	void setGain(const char *str);
	void setSpeed(const char *str);
	void setMode(const char *str);
	void setColor(const QColor &color);

protected:

private Q_SLOTS:

private:
	Label identity;
	Label lead;
	Label gain;
	Label speed;
	Label mode;

	void init();
};

#endif
