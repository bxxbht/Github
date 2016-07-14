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

#include <QtGui>
#include <QDebug>

#include "mainwindow.h"

MainWindow::MainWindow(Configuration *config, QWidget *parent)
    : QDialog(parent)
{
    QProcess::execute("mkdir -p cap");
    setWindowTitle("TTY Proxy Capture");
    setWindowIcon(QIcon(":/images/heart.svg"));

    createActions();
    createTrayIcon();
    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));

    this->config = config;

    QStringList iplist = config->getHostAddress().split(".");

    QLabel *ipAddrLabel = new QLabel("Server IP:");
    ipAddr0 = new QSpinBox(this);
    ipAddr0->setRange(127, 254);
    ipAddr0->setValue(iplist[0].toInt());
    ipAddr1 = new QSpinBox(this);
    ipAddr1->setRange(0, 254);
    ipAddr1->setValue(iplist[1].toInt());
    ipAddr2 = new QSpinBox(this);
    ipAddr2->setRange(0, 254);
    ipAddr2->setValue(iplist[2].toInt());
    ipAddr3 = new QSpinBox(this);
    ipAddr3->setRange(220, 250);
    ipAddr3->setValue(iplist[3].toInt());

    QHBoxLayout *ipAddrLayout = new QHBoxLayout();
    ipAddrLayout->setSpacing(0);
    ipAddrLayout->addWidget(ipAddr0);
    ipAddrLayout->addWidget(ipAddr1);
    ipAddrLayout->addWidget(ipAddr2);
    ipAddrLayout->addWidget(ipAddr3);

    serverStatus = false;
    setIcon(serverStatus);
    switchButton = new QPushButton("Start", this);
    switchButton->setFocus(Qt::OtherFocusReason);
    connect(switchButton, SIGNAL(clicked()), this, SLOT(save()));
    // connect(switchButton, SIGNAL(clicked()), this, SLOT(start()));
    // connect(switchButton, SIGNAL(clicked()), this, SLOT(stop()));

    txBytes = 0;
    QLabel *txCountLabel = new QLabel("TX:");
    txCount = new QLineEdit;
    txCount->setAlignment(Qt::AlignRight);
    txCount->setReadOnly(true);
    txTcpSocket = new QTcpSocket(this);
    txDecode = new DecodeChannel("cap/tx%1.dat");
    connect(txTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(txTcpSocket, SIGNAL(connected()), this, SLOT(connectedChannel()));
    connect(txTcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectedChannel()));
    connect(txTcpSocket, SIGNAL(readyRead()), this, SLOT(readTxChannel()));
    
    rxBytes = 0;
    QLabel *rxCountLabel = new QLabel("RX:");
    rxCount = new QLineEdit;
    rxCount->setAlignment(Qt::AlignRight);
    rxCount->setReadOnly(true);
    rxTcpSocket = new QTcpSocket(this);
    rxDecode = new DecodeChannel("cap/rx%1.dat");
    connect(rxTcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             this, SLOT(displayError(QAbstractSocket::SocketError)));
    connect(rxTcpSocket, SIGNAL(connected()), this, SLOT(connectedChannel()));
    connect(rxTcpSocket, SIGNAL(disconnected()), this, SLOT(disconnectedChannel()));
    connect(rxTcpSocket, SIGNAL(readyRead()), this, SLOT(readRxChannel()));

    QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(refreshCount()));
	timer->start(500);

    statusBar = new QStatusBar;
    statusBar->showMessage("TTY Proxy Capture is ready ...");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(ipAddrLabel, 0, 0);
    mainLayout->addLayout(ipAddrLayout, 0, 1, 1, 2);
    mainLayout->addWidget(switchButton, 0, 3);

    mainLayout->addWidget(txCountLabel, 1, 0);
    mainLayout->addWidget(txCount, 1, 1, 1, 2);

    mainLayout->addWidget(rxCountLabel, 2, 0);
    mainLayout->addWidget(rxCount, 2, 1, 1, 2);

    mainLayout->addWidget(statusBar, 3, 0, 1, 4);

    setLayout(mainLayout);

    trayIcon->show();
}

MainWindow::~MainWindow()
{
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    qDebug() << socketError;
    switch (socketError) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        QMessageBox::information(this, tr("TTY Proxy Capture"),
                                 tr("The host was not found. Please check the "
                                    "host name and port settings."));
        break;
    case QAbstractSocket::ConnectionRefusedError:
        QMessageBox::information(this, tr("TTY Proxy Capture"),
                                 tr("The connection was refused by the peer. "
                                    "Make sure the fortune server is running, "
                                    "and check that the host name and port "
                                    "settings are correct."));
        break;
    default:
        QMessageBox::information(this, tr("TTY Proxy Capture"),
                                 tr("The following error occurred: %1.")
                                 .arg(txTcpSocket->errorString()));
    }

    stop();
}

void MainWindow::connectedChannel()
{
    statusBar->showMessage("TTY Proxy Capture is connected");
}

void MainWindow::disconnectedChannel()
{
    statusBar->showMessage("TTY Proxy Capture is disconnected");
    stop();
}

void MainWindow::readTxChannel()
{
    QByteArray raw = txTcpSocket->readAll();
    if (raw.size() > 0) {
        txBytes += raw.size();
        decodeData(txDecode, raw);
    }
}

void MainWindow::readRxChannel()
{
    QByteArray raw = rxTcpSocket->readAll();
    if (raw.size() > 0) {
        rxBytes += raw.size();
        decodeData(rxDecode, raw);
    }
}

const char *MainWindow::checkData(QByteArray &buf)
{
    static const char *frameError = "corrupt frame";
    static const char *checksumError = "checksum error";

    quint8 tail = buf.at(buf.size()-1);
    if (tail != 0xaa)
        return frameError;

    quint8 checksum = buf.at(buf.size()-2);
    quint8 csum = 0;
    for (int i = 1; i < buf.size() - 2; i++) {
        csum += buf.at(i);
    }
    if (csum != checksum)
        return checksumError;
    return NULL;
}

void MainWindow::decodeData(DecodeChannel *decode, QByteArray &raw)
{
    const char *err;
    QByteArray &buf = decode->buf;
    int &stage = decode->stage;
    quint8 &size = decode->size;

    if (raw.size() < 0)
            return;

    for (int i = 0; i < raw.size(); i++) {
        quint8 data = raw.at(i);
        switch (stage) {
        case DecodeChannel::ID_STX:
            if (data == 0x55) {
                saveData(decode, "corrupt frame");
                stage = DecodeChannel::ID_VER;
                buf.clear();
                buf.append(data);
            } else {
                buf.append(data);
            }
            break;
        case DecodeChannel::ID_VER:
            buf.append(data);
            if (data == 0x04) {
                stage = DecodeChannel::ID_TSIZE;
            } else {
                stage = DecodeChannel::ID_STX;
            }
            break;
        case DecodeChannel::ID_TSIZE:
            buf.append(data);
            if (data >= 9) {
                stage = DecodeChannel::ID_CHNO;
                size = data - 9;
            } else {
                stage = DecodeChannel::ID_STX;
            }
            break;
        case DecodeChannel::ID_CHNO:
            buf.append(data);
            stage = DecodeChannel::ID_SADD;
            break;
        case DecodeChannel::ID_SADD:
            buf.append(data);
            stage = DecodeChannel::ID_TADD;
            break;
        case DecodeChannel::ID_TADD:
            buf.append(data);
            stage = DecodeChannel::ID_PID;
            break;
        case DecodeChannel::ID_PID:
            buf.append(data);
            stage = DecodeChannel::ID_DATA;
            break;
        case DecodeChannel::ID_DATA:
            size--;
            buf.append(data);
            if (size == 0)
                stage = DecodeChannel::ID_CSUM;
            break;
        case DecodeChannel::ID_CSUM:
            buf.append(data);
            stage = DecodeChannel::ID_ETX;
            break;
        case DecodeChannel::ID_ETX:
            buf.append(data);
            stage = DecodeChannel::ID_STX;
            err = checkData(buf);
            if (!err) {
                saveData(decode, err);
                buf.clear();
            }
            break;
        default:
            break;
        }
    }
}

void MainWindow::saveData(DecodeChannel *decode, const char *err)
{
    QByteArray &buf = decode->buf;
    QFile *file = decode->file;

    if (buf.size() > 0) {
            QDate date = QDate::currentDate();
            QTime time = QTime::currentTime();
            if (file->isOpen()) {
                if (file->size() > (64 * 1024 * 1024))
                    file->close();
                QStringList gzipArgs;
                gzipArgs << "-1";
                gzipArgs << file->fileName();
                QProcess::startDetached("gzip", gzipArgs);
            }

            if (!file->isOpen()) {
                QString suffix("-");
                suffix += date.toString("MMdd");
                suffix += time.toString("-hhmmss");
                QString filename = QString(decode->basename).arg(suffix);
                file->setFileName(filename);
                file->open(QIODevice::WriteOnly | QIODevice::Truncate);
            }

            QString prefix;
            prefix += date.toString("MM/dd-");
            prefix += time.toString("hh:mm:ss (");
            if (err)
                    prefix += err;
            prefix += ") >>> ";

            QByteArray stream;
            stream.reserve(4096);
            stream.append(prefix);
            char str[4];
            for (int i = 0; i < buf.size(); i++) {
                qsnprintf(str, 4, "%02x ", (unsigned char)buf.at(i));
                stream.append(str, 3);
            }
            stream.append('\n');
            file->write(stream);
    }
}

void MainWindow::refreshCount()
{
     txCount->setText(QString::number(txBytes));
     rxCount->setText(QString::number(rxBytes));
}

void MainWindow::save()
{
    QString s;

    s.sprintf("%d.%d.%d.%d",
            ipAddr0->value(),
            ipAddr1->value(),
            ipAddr2->value(),
            ipAddr3->value());

    if (s != config->getHostAddress()) {
    	config->setHostAddress(s);

    	config->save();

	    // statusBar->showMessage("Save proxy server config ...", 1000 * 3);

	    // {
		// QMessageBox messagebox;
		// messagebox.setText("Server IP is changed\nYou need to reinstall driver");
		// messagebox.exec();
	    // }
    }

    if (serverStatus) {
        serverStatus = false;
        switchButton->setText("Start");
        uninstall();
    } else {
        serverStatus = true;
        switchButton->setText("Stop");
        install();
    }

    setIcon(serverStatus);
}

void MainWindow::start()
{
    if (serverStatus == false) {
        serverStatus = true;
        switchButton->setText("Stop");
        install();
    	setIcon(serverStatus);
    }
}

void MainWindow::stop()
{
    if (serverStatus == true) {
        serverStatus = false;
        switchButton->setText("Start");
        uninstall();
    	setIcon(serverStatus);
    }
}

void MainWindow::install()
{
    txBytes = 0;
    txTcpSocket->connectToHost(config->getHostAddress(), 20001);
    txDecode->buf.clear();
    txDecode->stage = DecodeChannel::ID_STX;

    rxBytes = 0;
    rxTcpSocket->connectToHost(config->getHostAddress(), 20003);
    rxDecode->buf.clear();
    rxDecode->stage = DecodeChannel::ID_STX;
}

void MainWindow::uninstall()
{
    txTcpSocket->abort();
    txDecode->file->close();

    rxTcpSocket->abort();
    rxDecode->file->close();
}

/* {{{ */
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
        break;
    case QSystemTrayIcon::DoubleClick:
        serverStatus ? stop() : start();
        break;
    case QSystemTrayIcon::MiddleClick:
	// showNormal();
	// showMessage();
        break;
    default:
        ;
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (trayIcon->isVisible()) {
#if 0
        QMessageBox::information(this, tr("UT6000 Machine Proxy"),
                                 tr("The program will keep running in the "
                                    "system tray. To terminate the program, "
                                    "choose <b>Quit</b> in the context menu "
                                    "of the system tray entry."));
#endif
        hide();
        event->ignore();
    }
}

void MainWindow::setIcon(bool status)
{
    if (status) {
        trayIcon->setIcon(QIcon(":/images/heart.svg"));
        trayIcon->setToolTip("start");
    } else {
        trayIcon->setIcon(QIcon(":/images/stop.svg"));
        trayIcon->setToolTip("stop");
    }

}

void MainWindow::createActions()
{
    startAction = new QAction(tr("&Start"), this);
    connect(startAction, SIGNAL(triggered()), this, SLOT(start()));

    stopAction = new QAction(tr("Sto&p"), this);
    connect(stopAction, SIGNAL(triggered()), this, SLOT(stop()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("&Quit"), this);
    connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));
}

void MainWindow::createTrayIcon()
{
    trayIconMenu = new QMenu(this);
    trayIconMenu->addAction(startAction);
    trayIconMenu->addAction(stopAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
}
/* }}} */
