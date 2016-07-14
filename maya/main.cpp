/*
 * Maya - software update utility
 *
 * Copyright (c) 2015, longfeng.xiao <xlongfeng@126.com>
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

#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QDebug>
#include "dialog.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
#ifndef __arm__
    a.setStyle("windows");
#endif
    QFile file(":/qss/default.qss");
    file.open(QFile::ReadOnly);
    a.setStyleSheet(QLatin1String(file.readAll()));

    Dialog w;
#ifdef __arm__
    w.showFullScreen();
#endif
    w.show();
    
    return a.exec();
}
