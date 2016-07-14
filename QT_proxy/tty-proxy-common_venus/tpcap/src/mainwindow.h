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

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QByteArray>
#include <QDialog>
#include <QFile>
#include <QSystemTrayIcon>
#include <QTableWidget>
#include <QTcpSocket>

#include "configuration.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QStatusBar;
class QTableWidget;
class QTcpSocket;
class QFile;
QT_END_NAMESPACE

class DecodeChannel
{
public:
    enum {
        ID_STX,
        ID_VER,
        ID_TSIZE,
        ID_CHNO,
        ID_SADD,
        ID_TADD,
        ID_PID,
        ID_DATA,
        ID_CSUM,
        ID_ETX,
    };
    QByteArray buf;
    quint8 size;
    int stage;

    QFile *file;
    QString basename;
    QString filename;

    DecodeChannel(const char *pattern)
    {
    	buf.reserve(1024);
	file = new QFile;
	basename = pattern;
    }

    ~DecodeChannel()
    {
	delete file;
    }
};

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(Configuration *config, QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);
    const char *checkData(QByteArray &buf);
    void decodeData(DecodeChannel *decode, QByteArray &raw);
    void saveData(DecodeChannel *decode, const char *err);

private:
    void createActions();
    void createTrayIcon();

private Q_SLOTS:
    void displayError(QAbstractSocket::SocketError socketError);
    void connectedChannel();
    void disconnectedChannel();
    void readTxChannel();
    void readRxChannel();
    void refreshCount();
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
    void setIcon(bool status);
    void save();
    void start();
    void stop();
    void install();
    void uninstall();

private:
    Configuration *config;
    QSpinBox *ipAddr0;
    QSpinBox *ipAddr1;
    QSpinBox *ipAddr2;
    QSpinBox *ipAddr3;

    bool serverStatus;
    QPushButton *switchButton;

    qulonglong txBytes;
    QLineEdit *txCount;
    QTcpSocket *txTcpSocket;
    DecodeChannel *txDecode;

    qulonglong rxBytes;
    QLineEdit *rxCount;
    QTcpSocket *rxTcpSocket;
    DecodeChannel *rxDecode;

    QStatusBar *statusBar;

    QAction *startAction;
    QAction *stopAction;
    QAction *restoreAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif
