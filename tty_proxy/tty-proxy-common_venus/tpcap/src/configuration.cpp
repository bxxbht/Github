/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore>
#include <QDebug>

#include "configuration.h"

Configuration::Configuration(QObject *parent)
    : QObject(parent)
{
    machType = 0;
    machTypeList = new QStringList;
    *machTypeList << "Vitas"
                 << "G60-Common"
                 << "G60-SFDA";

    configFile = new QFile("/opt/tty-proxy/client.conf");
    if (configFile->open(QIODevice::ReadOnly)) {
        QTextStream in(configFile);
        while (!in.atEnd()) {
            QString l = in.readLine();
            if (l.contains("machine=")) {
                machType = l.mid(QString("machine=").length()).toInt();
            }
            if (l.contains("address=")) {
                hostAddress = l.mid(QString("address=").length());
            }
        }
        configFile->close();
    } else {
        qDebug() << "Cannot open file for reading:"
              << qPrintable(configFile->errorString());
    }

    if (machType < 0 || machType >= machTypeList->count()) {
        machType = 0;
        qDebug() << "Set default machine:" << (*machTypeList)[0];
        save();
    } else {
        qDebug() << "machine:" << (*machTypeList)[machType];
    }

    if (!hostAddress.contains(
            QRegExp("^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$"))) {
        hostAddress = "192.168.7.220";
        qDebug() << "Set default host address:" << hostAddress;
        save();
    } else {
        qDebug() << "host address:" << hostAddress;
    }
}

Configuration::~Configuration()
{
    delete configFile;
    delete machTypeList;
}

void Configuration::restore()
{

}

void Configuration::save()
{
    if (configFile->open(QIODevice::WriteOnly)) {
        QTextStream out(configFile);
        out << "machine=" << machType << endl;
        out << "address=" << hostAddress << endl;
        configFile->close();
    } else {
        qDebug() << "Cannot open file for writing:"
              << qPrintable(configFile->errorString());
    }
}

QStringList Configuration::getMachTypeList() const
{
    return *machTypeList;
}

void Configuration::setMachType(int machtype)
{
    machType = machtype;
}

void Configuration::setHostAddress(const QString hostaddr)
{
    hostAddress = hostaddr;
}
