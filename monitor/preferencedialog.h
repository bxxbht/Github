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

#ifndef PREFERENCE_DIALOG_H
#define PREFERENCE_DIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QDialogButtonBox;
class QTabWidget;
QT_END_NAMESPACE

class Demo0Tab : public QWidget
{
	Q_OBJECT

public:
	Demo0Tab(QWidget *parent = 0);
};

class Demo1Tab : public QWidget
{
	Q_OBJECT

public:
	Demo1Tab(QWidget *parent = 0);
};

class Demo2Tab : public QWidget
{
	Q_OBJECT

public:
	Demo2Tab(QWidget *parent = 0);
};

class Demo3Tab : public QWidget
{
	Q_OBJECT

public:
	Demo3Tab(QWidget *parent = 0);
};

class Demo4Tab : public QWidget
{
	Q_OBJECT

public:
	Demo4Tab(QWidget *parent = 0);
};

class Demo5Tab : public QWidget
{
	Q_OBJECT

public:
	Demo5Tab(QWidget *parent = 0);
};

class PreferenceDialog : public QDialog
{
	Q_OBJECT

public:
	PreferenceDialog(QWidget *parent = 0);

protected:
	void paintEvent(QPaintEvent *);

private:
	QTabWidget *tabWidget;
	QDialogButtonBox *buttonBox;
};

#endif
