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

#ifndef NBP_TREND
#define NBP_TREND

#include <QFrame>
#include <QTableWidget>

QT_BEGIN_NAMESPACE
class QTableWidget;
QT_END_NAMESPACE

class NbpTrend : public QFrame
{
	Q_OBJECT

public:
	NbpTrend(QWidget *parent = 0);
	~NbpTrend();
	void addNbpTrend(const QString &date, const QString &hr,
		const QString &pvc, const QString &spo2, const QString &pr, const QString &nbp);

protected:
	void paintEvent(QPaintEvent *);

private Q_SLOTS:

private:
	int addRow();

private:
	QTableWidget *tableWidget;
};

#endif
