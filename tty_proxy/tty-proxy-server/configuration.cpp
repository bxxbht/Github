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

#ifdef __arm__
#include <stdio.h>
#include <stdlib.h>
#endif
#include <QtCore>
#include <QSettings>
#include <QDebug>

#include "qextserialport.h"
#include "configuration.h"


Configuration::Configuration(QObject *parent)
    : QObject(parent)
{
#ifdef __arm__
    settings = new QSettings("/cfgdir/.tty-proxy-server.ini", QSettings::IniFormat, parent);
#else
    settings = new QSettings("tty-proxy-server.ini", QSettings::IniFormat, parent);
#endif

    machTypeList << "Vitas" << "G60-Common" << "G60-SFDA" << "Vitas-2";

    machType = settings->value("mach", "Vitas-2").toString();
    hostAddress = settings->value("ipaddr", "199.168.0.199").toString();
    macEther = settings->value("hwaddr", "00:01:02:03:24:dc").toString();

    if (!hostAddress.contains(
            QRegExp("^[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}.[0-9]{1,3}$"))) {
        hostAddress = "199.168.0.199";
    }
    
    if (!macEther.contains(
            QRegExp("^[0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}:[0-9a-f]{1,2}$"))) {
        macEther = "00:01:02:03:24:dc";
    }

    setupHostNetwork();
}

Configuration::~Configuration()
{
}

void Configuration::save()
{
    settings->setValue("mach", machType);
    settings->setValue("ipaddr", hostAddress);
    settings->setValue("hwaddr", macEther);
}

QStringList Configuration::getMachTypeList() const
{
    return machTypeList;
}

int Configuration::getMachTypeIndex()
{
    return machTypeList.indexOf(machType);
}

void Configuration::setMachType(const QString &machtype)
{
    this->machType = machtype;
}

void Configuration::setHostAddress(const QString hostaddr)
{
    if (hostaddr != hostAddress) {
        hostAddress = hostaddr;
        setupHostNetwork();
    }
}

void Configuration::setMacEther(const QString macether)
{
    if (macether != macEther) {
        macEther = macether;
        setupHostNetwork();
    }
}

void Configuration::setupHostNetwork()
{
#ifdef __arm__
    QProcess::execute("ifconfig", QStringList() << "eth0" << "down");
    usleep(1000*100);
    QProcess::execute("ifconfig", QStringList() << "eth0" << "hw" << "ether" << getMacEther());
    usleep(1000*100);
    QProcess::execute("ifconfig", QStringList() << "eth0" << getHostAddress());
    usleep(1000*100);
    QProcess::execute("ifconfig", QStringList() << "eth0" << "up");
    usleep(1000*500);
#endif
}
