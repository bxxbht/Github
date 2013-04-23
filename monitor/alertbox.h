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

#ifndef ALERT_BOX_H
#define ALERT_BOX_H

#include <QLabel>
#include <QList>

QT_BEGIN_NAMESPACE
class QString;
class QTimer;
QT_END_NAMESPACE

class AlertBox : public QLabel
{
	Q_OBJECT

public:
	enum AlertLevel {
		Informational,
		Notice,
		Warning,
		Critical,
	};

	struct AlertItem {
		AlertLevel level;
		QString message;
		int expire;
	};

	AlertBox(QWidget *parent = 0);
	AlertBox(const char *text, QWidget *parent = 0);
	~AlertBox();

	bool addAlert(const AlertItem &item);
	bool removeAlert(const AlertItem &item);

protected:
	virtual void paintEvent(QPaintEvent *);

private Q_SLOTS:
	void refresh();
	void blanking();

private:
	class AlertList {
	public:
		AlertList() { index = 0; }
		bool isEmpty() { return list.empty(); }
		const AlertItem &loop()
		{
			const AlertItem &item = list[index];
			index++;
			if (index >= list.count())
				index = 0;
		}

	private:
		QList<AlertItem> list;
		int index;
	};
	AlertList noticeList;
	AlertList warningList;
	AlertList criticalList;

	AlertItem defaultItem;
	AlertItem *effectiveItem;

	static const int refleshRate = 100;
	int refleshCounter;
	bool labelBlanking;
	int labelBlankingCounter;

	void initCounter();
};

#endif
