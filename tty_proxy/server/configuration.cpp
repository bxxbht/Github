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
#include <QDebug>

#include "qextserialport.h"
#include "configuration.h"

#if 0
#define DEVICE_MAJOR 20000

static Machine machine[MACH_TOTAL] =
{
    {
        "Vitas", MACH_VITAS, 5,
        {
            {
                .device = "/dev/ttySAC0",
                .desc = "console",
                .bandrate = BAUD115200,
                .major = DEVICE_MAJOR,
                .minor = 0,
            },
            {
                .device = "/dev/ttySAC1",
                .desc = "",
                .bandrate = BAUD115200,
                .major = DEVICE_MAJOR,
                .minor = 1,
            },
            {
                .device = "/dev/ttySAC2",
                .desc = "",
                .bandrate = BAUD38400,
                .major = DEVICE_MAJOR,
                .minor = 2,
            },
            {
                .device = "/dev/ttySAC3",
                .desc = "",
                .bandrate = BAUD19200,
                .major = DEVICE_MAJOR,
                .minor = 3,
            },
            {
                .device = "/dev/ttySYS",
                .desc = "",
                .bandrate = BAUD4800,
                .major = DEVICE_MAJOR,
                .minor = 4,
            },
        },
    },
    {
        "G60-Common", MACH_G60_COMMON, 6,
        {

        },
    },
    {
        "G60-SFDA", MACH_G60_SFDA, 7,
        {

        },
    },
};
#endif

Configuration::Configuration(QObject *parent)
    : QObject(parent)
{
#if 0
#ifdef __arm__
    cmdline = new QFile("/proc/cmdline");
#else
    cmdline = new QFile("cmdline");
#endif
    machType = "unknown";
    proxyDebug = false;
    if (cmdline->open(QIODevice::ReadOnly)) {
        QTextStream in(cmdline);
        QStringList list = in.readAll().split(" ");
        foreach (QString s, list) {
            if (s.contains("machtype=")) {
                machType = s.mid(QString("machtype=").length());
            }
            if (s.contains("proxydebug=")) {
                proxyDebug = s.mid(QString("proxydebug=").length()) == "1";
            }
        }
        cmdline->close();
    } else {
        qDebug() << "Cannot open file for reading:"
              << qPrintable(cmdline->errorString());
    }
#else
    machType = 0;
    machTypeList = new QStringList;
    *machTypeList << "Vitas"
                 << "G60-Common"
                 << "G60-SFDA";
#endif

#ifdef __arm__
    machFile = new QFile("/tmp/machine");
    if (machFile->open(QIODevice::ReadOnly)) {
        QTextStream in(machFile);
        while (!in.atEnd()) {
            QString l = in.readLine();
            if (l.contains("machine=")) {
                machType = l.mid(QString("machine=").length()).toInt();
            }
        }
        machFile->close();
    } else {
        qDebug() << "Cannot open file for reading:"
              << qPrintable(machFile->errorString());
    }
#endif

#ifdef __arm__
    configFile = new QFile("/cfgdir/.server.conf");
#else
    configFile = new QFile("server.conf");
#endif
    if (configFile->open(QIODevice::ReadOnly)) {
        QTextStream in(configFile);
        while (!in.atEnd()) {
            QString l = in.readLine();
            if (l.contains("address=")) {
                hostAddress = l.mid(QString("address=").length());
            }
            if (l.contains("mac_ether=")) {
                macEther = l.mid(QString("mac_ether=").length());
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
    
    if (!macEther.contains(
            QRegExp("^[0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}:[0-9]{1,2}:[0-9a-f]{1,2}$"))) {
        macEther = "00:01:02:03:24:dc";
        qDebug() << "Set default mac ether:" << macEther;
        save();
    } else {
        qDebug() << "host mac ether:" << macEther;
    }

    setupHostNetwork();
}

Configuration::~Configuration()
{
    delete configFile;
#if 0
    delete cmdline;
#else
    delete machTypeList;
#endif
}

void Configuration::restore()
{

}

void Configuration::save()
{
    if (configFile->open(QIODevice::WriteOnly)) {
        QTextStream out(configFile);
        out << "address=" << hostAddress << endl;
        out << "mac_ether=" << macEther << endl;
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
