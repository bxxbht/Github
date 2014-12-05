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
    setWindowTitle("UT6000 Machine Proxy");
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
    ipAddr3->setRange(100, 240);
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

    statusBar = new QStatusBar;
    statusBar->showMessage("Proxy server is ready ...");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(ipAddrLabel, 0, 0);
    mainLayout->addLayout(ipAddrLayout, 0, 1, 1, 2);
    mainLayout->addWidget(switchButton, 0, 3);
    mainLayout->addWidget(statusBar, 1, 0, 1, 4);
    setLayout(mainLayout);

    trayIcon->show();
}

MainWindow::~MainWindow()
{
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
    statusBar->showMessage("Proxy server is started ...");
    QProcess::execute("/opt/tty-proxy/kapp");
    QProcess::execute("rmmod", QStringList() << "ttyp");
    QProcess::execute("insmod", QStringList() << "/opt/tty-proxy/ttyp.ko"
    		<< QString("serverip=%1").arg(config->getHostAddress()));
    QProcess::execute("/opt/tty-proxy/mkln");
}

void MainWindow::uninstall()
{
    statusBar->showMessage("Proxy server is stopped ...");
    QProcess::execute("/opt/tty-proxy/kapp");
    QProcess::execute("/opt/tty-proxy/rmln");
    QProcess::execute("rmmod", QStringList() << "ttyp");
}

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
