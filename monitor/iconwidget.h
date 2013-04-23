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

#ifndef ICON_WIDGET_H
#define ICON_WIDGET_H

#include <QWidget>
#include <QIcon>

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class IconWidget : public QWidget
{
	Q_OBJECT

public:
	IconWidget(QWidget *parent = 0);
	IconWidget(const QPixmap &pixmap, QWidget *parent = 0);
	IconWidget(const QIcon &other, QWidget *parent = 0);
	IconWidget(const QString &fileName, QWidget *parent = 0);
	~IconWidget();
	void addFile(const QString &fileName, const QSize &size = QSize(),
			QIcon::Mode mode = QIcon::Normal, QIcon::State state = QIcon::Off);
	inline const QIcon *iconRef() const { return icon; }
	inline void setMode(QIcon::Mode _mode) { mode = _mode; }
	inline void setState(QIcon::State _state) { state = _state; }
	inline void setAlign(Qt::Alignment _align) { align = _align; }

protected:
	void paintEvent(QPaintEvent *);

private Q_SLOTS:

private:
	QIcon *icon;
	QIcon::Mode mode;
	QIcon::State state;
	Qt::Alignment align;
	inline void init()
	{
		mode = QIcon::Normal;
		state = QIcon::Off;
		align = Qt::AlignCenter;
	}
};

#endif
