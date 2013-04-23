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
#include "digitalclock.h"
#include "shortcutmenu.h"
#include "userinfo.h"
#include "preferencedialog.h"

ShortcutMenu::ShortcutMenu(QWidget *parent)
	: QWidget(parent)
{
	// setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	setFixedHeight(36);

	UserInfo *userInfo = new UserInfo;
	userInfo->setName("Alice.demo");

	mainMenuButton = new QPushButton(QIcon("images/preferences-system.png"), "");
	connect(mainMenuButton, SIGNAL(clicked()), this, SLOT(popMainMenu()));
	DigitalClock *digitalClock = new DigitalClock;

	mainMenu = new QDockWidget(tr("Preference"), parent);
	mainMenu->setTitleBarWidget(new QWidget);
	mainMenu->setAllowedAreas(Qt::NoDockWidgetArea);
	mainMenu->setFloating(true);
	mainMenu->hide();
	PreferenceDialog *mainPanel = new PreferenceDialog;
	mainMenu->setWidget(mainPanel);

	QHBoxLayout *layout = new QHBoxLayout;
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setSpacing(0);

	layout->addWidget(userInfo);
	layout->addStretch();
	layout->addWidget(mainMenuButton);
	layout->addWidget(digitalClock);

	setLayout(layout);
}

ShortcutMenu::~ShortcutMenu()
{
}

// QSize ShortcutMenu::sizeHint() const
// {
	// return QSize(800, 36);
// }

void ShortcutMenu::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	painter.setPen(QPen(QColor("#3c3a39"), 1));
	painter.drawLine(rect().topLeft(), rect().topRight());

	QLinearGradient gradient(rect().topLeft(), rect().bottomLeft());
	gradient.setColorAt(0.0, QColor("#2b2b2a"));
	gradient.setColorAt(1.0, QColor("#050404"));
	painter.fillRect(rect().adjusted(0, 1, 0, 0), QBrush(gradient));
}

void ShortcutMenu::popMainMenu()
{
	QPoint point = mapToGlobal(QPoint(0,0));
	const QSize &size = mainMenu->sizeHint();
	point.setY(point.y() - size.height());
	mainMenu->move(point);
	mainMenu->toggleViewAction()->trigger();
}
