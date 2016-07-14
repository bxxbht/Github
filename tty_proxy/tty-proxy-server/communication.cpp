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

#include <termios.h>
#include <QtGui>
#include <QDebug>

#include "communication.h"

Communication::Communication(const QString &device, BaudRateType bandrate, quint16 port)
{
    clientConnectStatus = 0;
    totalReceivedBytes = 0;
    totalSentBytes = 0;
    totalReceivedBytesPre = 0;
    totalSentBytesPre = 0;
    receivingRate = 0;
    sendingRate = 0;
    updateTimer = new QTimer(this);

    ttyBandrate = bandrate;
    tcpPort = port;
    portDevice = new QextSerialPort(device, QextSerialPort::EventDriven);
    portDevice->setBaudRate(bandrate);
    portDevice->setFlowControl(FLOW_OFF);
    portDevice->setParity(PAR_NONE);
    portDevice->setDataBits(DATA_8);
    portDevice->setStopBits(STOP_1);

    tcpSocket = NULL;
    tcpServer = new QTcpServer(this);
    // tcpServer->setMaxPendingConnections(1);
}

Communication::~Communication()
{
    stop();
    delete tcpServer;
    delete portDevice;
    delete updateTimer;
}

void Communication::clientConnection()
{
    if (clientConnectStatus) {
        qDebug() << "Client is already connecting, so skip other connections";
        return;
    }

    tcpSocket = tcpServer->nextPendingConnection();
    if (tcpSocket) {
        clientConnectStatus = 1;
        connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnection()));
        connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(clientReadData()));

        if (portDevice->open(QIODevice::ReadWrite) == true) {
            // qDebug() << "Start listening for data on" << portDevice->portName();
            connect(portDevice, SIGNAL(readyRead()), this, SLOT(portReadData()));
        } else {
            qDebug() << "Failed to open device:" << portDevice->errorString();
        }

        emit statisticsUpdate(this, privData);
        // qDebug() << "Client connected";
    }
}

void Communication::clientDisconnection()
{
    if (clientConnectStatus) {
        clientConnectStatus = 0;
        portDevice->close();
        disconnect(portDevice, SIGNAL(readyRead()), this, SLOT(portReadData()));
        disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(clientReadData()));
        disconnect(tcpSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnection()));
        emit statisticsUpdate(this, privData);
        // qDebug() << "Client disconnect";
    }
}

void Communication::clientReadData()
{
    qint64 bytes = tcpSocket->bytesAvailable();
    QByteArray block(bytes, 0);
    int read = tcpSocket->read(block.data(), block.size());
    if (read > 0) {
        // qDebug() << "clientReadData()" << bytes << read;
        portDevice->write(block.data(), block.size());
        totalSentBytes += bytes;
    }
}

void Communication::portReadData()
{
    qint64 bytes = portDevice->bytesAvailable();
    QByteArray block(bytes, 0);
    int read = portDevice->read(block.data(), block.size());
    if (read > 0) {
        // qDebug() << "portReadData()" << bytes << read;
        if (clientConnectStatus) {
            tcpSocket->write(block.data(), block.size());
            totalReceivedBytes += bytes;
        }
    }
}

void Communication::statistics()
{
    totalSentBytesPre = totalSentBytesPre <= totalSentBytes ? totalSentBytesPre : 0;
    totalReceivedBytesPre = totalReceivedBytesPre <= totalReceivedBytes ? totalReceivedBytesPre : 0;
    receivingRate = totalReceivedBytes - totalReceivedBytesPre;
    sendingRate = totalSentBytes - totalSentBytesPre;
    totalReceivedBytesPre = totalReceivedBytes;
    totalSentBytesPre = totalSentBytes;
    emit statisticsUpdate(this, privData);
}

bool Communication::start()
{
    if (tcpServer->listen(QHostAddress::Any, tcpPort)) {
        connect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnection()));
        // qDebug() << "Start listenning at port" << tcpPort;
    } else {
        qDebug() << "Failed to bind to port" << tcpPort;
        return false;
    }

    connect(updateTimer, SIGNAL(timeout()), this, SLOT(statistics()));
    updateTimer->start(1000);

    return true;
}

void Communication::stop()
{
    disconnect(updateTimer, SIGNAL(timeout()), this, SLOT(statistics()));
    updateTimer->stop();
    disconnect(portDevice, SIGNAL(readyRead()), this, SLOT(portReadData()));
    portDevice->close();

    tcpServer->close();
    disconnect(tcpServer, SIGNAL(newConnection()), this, SLOT(clientConnection()));
    if (clientConnectStatus) {
        clientConnectStatus = 0;
        tcpSocket->disconnectFromHost();
        disconnect(tcpSocket, SIGNAL(readyRead()), this, SLOT(clientReadData()));
        disconnect(tcpSocket, SIGNAL(disconnected()), this, SLOT(clientDisconnection()));
    }
}

void Communication::restart()
{
    totalReceivedBytes = 0;
    totalSentBytes = 0;
    totalReceivedBytesPre = 0;
    totalSentBytesPre = 0;
    receivingRate = 0;
    sendingRate = 0;
    stop();
    start();
}

QString Communication::portName() const
{
    return portDevice->portName();
}

QString Communication::getPortRate() const
{
    return QString("%1 bps").arg(portDevice->baudRateBps());
}

QString Communication::getSocketPort() const
{
    return QString::number(tcpPort);
}

QString Communication::getRecievedBytes() const
{
    if (totalReceivedBytes < 1024 * 100)
        return QString::number(totalReceivedBytes);
    else if (totalReceivedBytes < 1024 * 1024)
        return QString("%1 KB").arg(totalReceivedBytes >> 10);
    else
        return QString("%1 MB").arg(totalReceivedBytes >> 20);
}

QString Communication::getSentBytes() const
{
    if (totalSentBytes < 1024 * 100)
        return QString::number(totalSentBytes);
    else if (totalSentBytes < 1024 * 1024)
        return QString("%1 KB").arg(totalSentBytes >> 10);
    else
        return QString("%1 MB").arg(totalSentBytes >> 20);
}

QString Communication::getRecievingRate() const
{
    if (receivingRate < 1024 * 100)
        return QString("%1 B/s").arg(receivingRate);
    else if (receivingRate < 1024 * 1024)
        return QString("%1 KB/s").arg(receivingRate >> 10);
    else
        return QString("%1 MB/s").arg(receivingRate >> 20);
}

QString Communication::getSendingRate() const
{
    if (sendingRate < 1024 * 100)
        return QString("%1 B/s").arg(sendingRate);
    else if (sendingRate < 1024 * 1024)
        return QString("%1 KB/s").arg(sendingRate >> 10);
    else
        return QString("%1 MB/s").arg(sendingRate >> 20);
}

bool Communication::isConnected() const
{
    return clientConnectStatus;
}
