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

#include <QBoxLayout>
#include <QLineEdit>

#include "iconwidget.h"
#include "userinfo.h"

UserInfo::UserInfo(QWidget *parent)
	: QFrame(parent)
{
	IconWidget *icon = new IconWidget("images/user-info.png");
	icon->setFixedSize(32, 32);

	nameEdit = new QLineEdit;
	nameEdit->setReadOnly(true);

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->setContentsMargins(0, 0, 0, 0);
	hLayout->insertSpacing(0, 8);
	hLayout->addWidget(icon);
	hLayout->addWidget(nameEdit);
	hLayout->addStretch();

	setLayout(hLayout);
}

UserInfo::~UserInfo()
{

}

void UserInfo::setName(const char *str)
{
	nameEdit->setText(str);
}
