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

#ifndef COMMUNICATION_H
#define COMMUNICATION_H

#include <QObject>
#include <QTimer>
#include <QTcpServer>
#include <QTcpSocket>

#include "qextserialport.h"

QT_BEGIN_NAMESPACE
QT_END_NAMESPACE

class Communication : public QObject

{
    Q_OBJECT

public:
    Communication(const QString &device, BaudRateType bandrate = BAUD115200, quint16 port = 0);
    ~Communication();
    bool start();
    void stop();
    QString portName() const;
    bool isConnected() const;
    QString getPortRate() const;
    QString getSocketPort() const;
    QString getRecievedBytes() const;
    QString getSentBytes() const;
    QString getRecievingRate() const;
    QString getSendingRate() const;
    inline void setPrivData(int priv) { privData = priv; }

Q_SIGNALS:
    void statisticsUpdate(Communication *port, int priv);

private Q_SLOTS:
    void restart();
    void clientConnection();
    void clientDisconnection();
    void clientReadData();
    void portReadData();
    void statistics();

private:
    QextSerialPort *portDevice;
    quint32 ttyBandrate;
    QTcpServer *tcpServer;
    QTcpSocket *tcpSocket;
    quint16 tcpPort;
    bool clientConnectStatus;
    quint32 totalReceivedBytes;
    quint32 totalSentBytes;
    quint32 totalReceivedBytesPre;
    quint32 totalSentBytesPre;
    quint32 receivingRate;
    quint32 sendingRate;
    QTimer *updateTimer;
    int privData;
};

#endif
