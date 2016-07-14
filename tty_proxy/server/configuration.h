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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QObject>

QT_BEGIN_NAMESPACE
class QFile;
class QString;
QT_END_NAMESPACE

enum MachType {
    MACH_VITAS = 0,
    MACH_G60_COMMON,
    MACH_G60_SFDA,
    MACH_TOTAL,
};

#if 0
struct ProxyDevice {
    char *device;
    char *desc;
    BaudRateType bandrate;
    quint16 major;
    quint16 minor;
};

struct Machine {
    char *desc;
    MachType type;
    int number;
    ProxyDevice device[16];
};
#endif

class Configuration : public QObject
{
    Q_OBJECT

public:
    Configuration(QObject * parent = 0);
    ~Configuration();
    void restore();
    void save();
    QStringList getMachTypeList() const;
    inline int getMachType() const { return machType; };
    void setMachType(int machtype);
    inline bool isProxyDebug() const { return proxyDebug; };
    inline QString getHostAddress() const { return hostAddress; };
    inline QString getMacEther() const { return macEther; };
    void setHostAddress(const QString hostaddr);
    void setMacEther(const QString macether);

protected:
    void setupHostNetwork();

private Q_SLOTS:

private:
    QFile *cmdline;
    QFile *machFile;
    QFile *configFile;
    int machType;
    QStringList *machTypeList;
    bool proxyDebug;
    QString hostAddress;
    QString macEther;
};

#endif
