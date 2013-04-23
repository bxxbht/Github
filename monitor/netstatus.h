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

#ifndef NET_STATUS_H
#define NET_STATUS_H

#include "iconwidget.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class NetStatus : public IconWidget
{
	Q_OBJECT

public:
	NetStatus(QWidget *parent = 0);
	virtual QSize minimumSizeHint() const;
	void setStatus(int state);

protected:

private Q_SLOTS:

private:
};


#endif
