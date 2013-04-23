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

#ifndef CHANNEL_PANEL_H
#define CHANNEL_PANEL_H

#include <QFrame>

QT_BEGIN_NAMESPACE
class EcgChannel;
QT_END_NAMESPACE

class ChannelPanel : public QFrame
{
	Q_OBJECT

public:
	ChannelPanel(QWidget *parent = 0);
	~ChannelPanel();

protected:
	virtual void paintEvent(QPaintEvent *event);

private Q_SLOTS:
	void demo();

private:
	EcgChannel *ecgChannel1;
	EcgChannel *ecgChannel2;
	EcgChannel *ecgChannel3;
	EcgChannel *ecgChannel4;
	EcgChannel *ecgChannel5;
	EcgChannel *ecgChannel6;
	EcgChannel *ecgChannel7;
	EcgChannel *ecgChannel8;
};

#endif
