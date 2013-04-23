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

#include <QtGui>
#include <QDebug>

#include "alarmbar.h"
#include "channelpanel.h"
#include "parameterpanel.h"
#include "statusbar.h"
#include "shortcutmenu.h"
#include "tabdialog.h"
#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	// : QMainWindow(parent, Qt::FramelessWindowHint)
{
	// setAttribute(Qt::WA_TranslucentBackground, true);

	QWidget *centralWidget = new QWidget(this);
	QGridLayout *layout = new QGridLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	AlarmBar *alarmBar = new AlarmBar(this);
	StatusBar *statusBar = new StatusBar(this);
	// TabDialog *channelPanel = new TabDialog(".");
	ChannelPanel *channelPanel = new ChannelPanel(this);
	ParameterPanel *parameterPanel = new ParameterPanel(this);
	ShortcutMenu *shortcutMenu = new ShortcutMenu(this);

	layout->addWidget(alarmBar, 0, 0);
	layout->addWidget(statusBar, 0, 1);
	layout->addWidget(channelPanel, 1, 0);
	layout->addWidget(parameterPanel, 1, 1);
	layout->addWidget(shortcutMenu, 2, 0, 1, 2);

	centralWidget->setLayout(layout);

	setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
}

QSize MainWindow::sizeHint() const
{
	return QSize(800, 600);
}
