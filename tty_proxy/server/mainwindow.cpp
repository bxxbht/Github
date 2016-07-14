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

#include "combobox.h"
#include "spinbox.h"
#include "mainwindow.h"

MainWindow::MainWindow(Configuration *config, QWidget *parent)
    : QDialog(parent)
{
    this->config = config;
    QLabel *machineLabel = new QLabel("Machine:", this);
    machine = new ComboBox(this);
    machine->setFocusPolicy(Qt::NoFocus);
    machine->addItems(config->getMachTypeList());
    machine->setCurrentIndex(config->getMachType());

#ifndef __arm__
    quitButton = new QPushButton(tr("&Quit"), this);
    quitButton->setDisabled(true);
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
#endif

    resetButton = new QPushButton("&Reset", this);

    QStringList iplist = config->getHostAddress().split(".");

    QLabel *ipAddrLabel = new QLabel("IP Address:", this);
    ipAddr0 = new SpinBox(this);
    ipAddr0->setRange(127, 254);
    ipAddr0->setValue(iplist[0].toInt());
    ipAddr1 = new SpinBox(this);
    ipAddr1->setRange(0, 254);
    ipAddr1->setValue(iplist[1].toInt());
    ipAddr2 = new SpinBox(this);
    ipAddr2->setRange(0, 254);
    ipAddr2->setValue(iplist[2].toInt());
    ipAddr3 = new SpinBox(this);
    ipAddr3->setRange(220, 250);
    ipAddr3->setValue(iplist[3].toInt());

    QHBoxLayout *ipAddrLayout = new QHBoxLayout;
    ipAddrLayout->setSpacing(0);
    ipAddrLayout->addWidget(ipAddr0);
    ipAddrLayout->addWidget(ipAddr1);
    ipAddrLayout->addWidget(ipAddr2);
    ipAddrLayout->addWidget(ipAddr3);

#if 0
    QStringList maclist = config->getMacEther().split(":");

    QLabel *macAddrLabel = new QLabel("MAC Address:");
    macAddr0 = new SpinBox(this);
    macAddr0->setRange(0, 99);
    macAddr0->setValue(maclist[0].toInt());
    macAddr1 = new SpinBox(this);
    macAddr1->setRange(0, 99);
    macAddr1->setValue(maclist[1].toInt());
    macAddr2 = new SpinBox(this);
    macAddr2->setRange(0, 99);
    macAddr2->setValue(maclist[2].toInt());
    macAddr3 = new SpinBox(this);
    macAddr3->setRange(0, 99);
    macAddr3->setValue(maclist[3].toInt());
    macAddr4 = new SpinBox(this);
    macAddr4->setRange(0, 99);
    macAddr4->setValue(maclist[4].toInt());
    macAddr5 = new SpinBox(this);
    macAddr5->setRange(0, 99);
    macAddr5->setValue(maclist[5].toInt());

    QHBoxLayout *macAddrLayout = new QHBoxLayout();
    macAddrLayout->setSpacing(0);
    macAddrLayout->addWidget(macAddr0);
    macAddrLayout->addWidget(macAddr1);
    macAddrLayout->addWidget(macAddr2);
    macAddrLayout->addWidget(macAddr3);
    macAddrLayout->addWidget(macAddr4);
    macAddrLayout->addWidget(macAddr5);
#endif

    applyButton = new QPushButton("&Apply", this);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    tableWidget = new QTableWidget(0, 7, this);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setHorizontalHeaderLabels(
            QStringList() << tr("Device")
                          << tr("Baudrate")
                          << tr("Total Received")
                          << tr("Total Sent")
                          << tr("Receiving")
                          << tr("Sending")
                          << tr("Status"));

    deviceDescription = new QTableWidget(0, 2, this);
    deviceDescription->setFocusPolicy(Qt::NoFocus);
    deviceDescription->horizontalHeader()->setResizeMode(1, QHeaderView::Stretch);
    deviceDescription->setAlternatingRowColors(true);
    deviceDescription->setHorizontalHeaderLabels(
            QStringList() << tr("Device")
                          << tr("Description"));
    deviceDescription->setColumnWidth(0, 200);

#ifdef __arm__
    if (config->getMachType() == MACH_VITAS) {
        addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
        addProxy(&module0Port, "/dev/ttySAC1", BAUD115200, 1, "Standard parameters of the module");
        addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
        addProxy(&module1Port, "/dev/ttySAC3", BAUD19200, 3, "CO2 module");
        addProxy(&systemPort, "/dev/ttySYS", BAUD4800, 4, "System board");
    } else if (config->getMachType() == MACH_G60_COMMON) {
        addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
        addProxy(&module0Port, "/dev/ttySAC1", BAUD500000, 1, "Built-in plug-in cage");
        addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
        addProxy(&module1Port, "/dev/ttySAC3", BAUD500000, 3, "External plug-in cage");
        addProxy(&systemPort, "/dev/ttyS0", BAUD4800, 4, "System board");
        addProxy(&tsPort, "/dev/ttyS1", BAUD9600, 5, "Touchscreen");
    } else if (config->getMachType() == MACH_G60_SFDA) {
        addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
        addProxy(&systemPort, "/dev/ttySAC1", BAUD4800, 1, "System board");
        addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
        addProxy(&tsPort, "/dev/ttySAC3", BAUD9600, 3, "Touchscreen");
        addProxy(&module0Port, "/dev/ttySS0", BAUD500000, 4, "Built-in plug-in cage");
        addProxy(&module1Port, "/dev/ttySS1", BAUD500000, 5, "Built-in plug-in cage");
        tableWidget->hideRow(5);
        deviceDescription->hideRow(5);
        addProxy(&module2Port, "/dev/ttySS2", BAUD500000, 6, "External plug-in cage");
        addProxy(&module3Port, "/dev/ttySS3", BAUD500000, 7, "External plug-in cage");
    }
#else
    addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
    addProxy(&systemPort, "/dev/ttySAC1", BAUD4800, 1, "System board");
    addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
    addProxy(&tsPort, "/dev/ttySAC3", BAUD9600, 3, "Touchscreen");
    addProxy(&module0Port, "/dev/ttySS0", BAUD500000, 4, "Built-in plug-in cage");
    addProxy(&module1Port, "/dev/ttySS1", BAUD500000, 5, "Built-in plug-in cage");
    tableWidget->hideRow(5);
    deviceDescription->hideRow(5);
    addProxy(&module2Port, "/dev/ttySS2", BAUD500000, 6, "External plug-in cage");
    addProxy(&module3Port, "/dev/ttySS3", BAUD500000, 7, "External plug-in cage");
#endif

    statusBar = new QStatusBar(this);
    statusBar->showMessage("Proxy server is starting ...");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(machineLabel, 0, 0);
    mainLayout->addWidget(machine, 0, 1);
#ifndef __arm__
    mainLayout->addWidget(quitButton, 0, 7);
#endif
    mainLayout->addWidget(ipAddrLabel, 1, 0);
    mainLayout->addLayout(ipAddrLayout, 1, 1, 1, 2);
#if 0
    mainLayout->addWidget(macAddrLabel, 2, 0);
    mainLayout->addLayout(macAddrLayout, 2, 1, 1, 2);
    mainLayout->addWidget(applyButton, 2, 3);
    mainLayout->addWidget(resetButton, 2, 7);
#else
    mainLayout->addWidget(applyButton, 1, 3);
    mainLayout->addWidget(resetButton, 1, 7);
#endif
    mainLayout->addWidget(tableWidget, 3, 0, 1, 8);
    mainLayout->addWidget(deviceDescription, 4, 0, 1, 8);
    mainLayout->addWidget(statusBar, 5, 0, 1, 8);
    setLayout(mainLayout);
    loadStyleSheet("default");
    // loadStyleSheet("suresign");
}

MainWindow::~MainWindow()
{
}

void MainWindow::addRow()
{
    {
        int row = tableWidget->rowCount();

        tableWidget->insertRow(row);

        QTableWidgetItem *item0 = new QTableWidgetItem;
        item0->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget->setItem(row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 2, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 3, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem;
        item4->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 4, item4);

        QTableWidgetItem *item5 = new QTableWidgetItem;
        item5->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        tableWidget->setItem(row, 5, item5);

        QTableWidgetItem *item6 = new QTableWidgetItem;
        item6->setTextAlignment(Qt::AlignCenter | Qt::AlignVCenter);
        tableWidget->setItem(row, 6, item6);
    }

    {
        int row = deviceDescription->rowCount();

        deviceDescription->insertRow(row);

        QTableWidgetItem *item0 = new QTableWidgetItem;
        item0->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        deviceDescription->setItem(row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        deviceDescription->setItem(row, 1, item1);
    }
}

void MainWindow::addProxy(Communication **pport, const char *device,
        BaudRateType bandrate, quint16 offset, const char *desc)
{
    *pport = new Communication(device, bandrate, 20000+offset);
    (*pport)->start();
    addRow();
    updateContent(*pport, offset);
    deviceDescription->item(offset, 0)->setText((*pport)->portName());
    deviceDescription->item(offset, 1)->setText(desc);
    (*pport)->setPrivData(offset);
    connect(*pport, SIGNAL(statisticsUpdate(Communication *, int)),
            this, SLOT(updateContent(Communication *, int)));
    connect(resetButton, SIGNAL(clicked()), *pport, SLOT(restart()));
}

void MainWindow::loadStyleSheet(const QString &sheetName)
{
    QFile file("qss/" + sheetName.toLower() + ".qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());

    // ui.styleTextEdit->setPlainText(styleSheet);
    qApp->setStyleSheet(styleSheet);
    // ui.applyButton->setEnabled(false);
}

void MainWindow::updateContent(Communication *port, int row)
{
    tableWidget->item(row, 0)->setText(port->portName());
    tableWidget->item(row, 1)->setText(port->getPortRate());
    tableWidget->item(row, 2)->setText(port->getRecievedBytes());
    tableWidget->item(row, 3)->setText(port->getSentBytes());
    tableWidget->item(row, 4)->setText(port->getRecievingRate());
    tableWidget->item(row, 5)->setText(port->getSendingRate());
    tableWidget->item(row, 6)->setForeground(port->isConnected() ?
            QBrush(QColor(203, 12, 41)) : QBrush(QColor(26, 200, 62)));
    tableWidget->item(row, 6)->setText(port->isConnected() ?
            "connected" : "disconnected");
}

void MainWindow::apply()
{
    QString s;
    bool machtypeischanged = false;

    if (machine->currentIndex() != config->getMachType()) {
        machtypeischanged = true;
        config->setMachType(machine->currentIndex());
    }

    s.sprintf("%d.%d.%d.%d",
            ipAddr0->value(),
            ipAddr1->value(),
            ipAddr2->value(),
            ipAddr3->value());
    config->setHostAddress(s);

#if 0
    s.sprintf("%02d:%02d:%02d:%02d:%02d:%02d",
            macAddr0->value(),
            macAddr1->value(),
            macAddr2->value(),
            macAddr3->value(),
            macAddr4->value(),
            macAddr5->value());
    config->setMacEther(s);
#else
    QStringList maclist = config->getMacEther().split(":");
    maclist[5] = QString::number(ipAddr3->value(), 16);
    config->setMacEther(maclist.join(":"));
#endif

    config->save();

    statusBar->showMessage("Save proxy server config ...", 1000 * 3);

    if (machtypeischanged) {
        QMessageBox messagebox;
        messagebox.setText("Machine type is changed, reboot system now");
        messagebox.exec();
#ifdef __arm__
        QProcess::execute("reboot");
#endif
    }
}
