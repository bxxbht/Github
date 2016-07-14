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
    machine->addItems(config->getMachTypeList());
    machine->setCurrentIndex(config->getMachTypeIndex());

#ifdef __arm__
    quitButton = new QPushButton(tr("&Quit"), this);
    connect(quitButton, SIGNAL(clicked()), this, SLOT(quit()));
#else
    quitButton = new QPushButton(tr("&Quit"), this);
    connect(quitButton, SIGNAL(clicked()), qApp, SLOT(quit()));
#endif

    resetButton = new QPushButton("&Reconnect", this);

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
    ipAddr3->setRange(100, 199);
    ipAddr3->setValue(iplist[3].toInt());

    QHBoxLayout *ipAddrLayout = new QHBoxLayout;
    ipAddrLayout->setSpacing(0);
    ipAddrLayout->addWidget(ipAddr0);
    ipAddrLayout->addWidget(ipAddr1);
    ipAddrLayout->addWidget(ipAddr2);
    ipAddrLayout->addWidget(ipAddr3);

    applyButton = new QPushButton("&Apply", this);
    connect(applyButton, SIGNAL(clicked()), this, SLOT(apply()));

    tableWidget = new QTableWidget(0, 7, this);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    tableWidget->setAlternatingRowColors(true);
    tableWidget->setShowGrid(false);
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
    deviceDescription->setSelectionBehavior(QTableWidget::SelectRows);
    deviceDescription->setAlternatingRowColors(true);
    deviceDescription->setShowGrid(false);
    deviceDescription->setHorizontalHeaderLabels(
            QStringList() << tr("Device")
                          << tr("Description"));
    deviceDescription->setColumnWidth(0, 200);

#ifdef __arm__
    if (config->getMachType() == "Vitas") {
        addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
        addProxy(&module0Port, "/dev/ttySAC1", BAUD115200, 1, "Standard parameters of the module");
        addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
        addProxy(&module1Port, "/dev/ttySAC3", BAUD19200, 3, "CO2 module");
        addProxy(&systemPort, "/dev/ttySYS", BAUD4800, 4, "System board");
    } else if (config->getMachType() == "G60-Common") {
        addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
        addProxy(&module0Port, "/dev/ttySAC1", BAUD500000, 1, "Built-in plug-in cage");
        addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
        addProxy(&module1Port, "/dev/ttySAC3", BAUD500000, 3, "External plug-in cage");
        addProxy(&systemPort, "/dev/ttyS0", BAUD4800, 4, "System board");
        addProxy(&tsPort, "/dev/ttyS1", BAUD9600, 5, "Touchscreen");
    } else if (config->getMachType() == "G60-SFDA") {
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
    } else { // vitas-2
        addProxy(&consolePort, "/dev/ttySAC0", BAUD115200, 0, "Console");
        addProxy(&module0Port, "/dev/ttySAC1", BAUD460800, 1, "Standard parameters of the module");
        addProxy(&printerPort, "/dev/ttySAC2", BAUD38400, 2, "Printer");
        addProxy(&module1Port, "/dev/ttySAC3", BAUD19200, 3, "CO2 module");
        addProxy(&systemPort, "/dev/ttySYS", BAUD4800, 4, "System board");
        //addProxy(&module3Port, "/dev/ttySS0", BAUD4800, 7, "ECG Sync Out");
        //addProxy(&module2Port, "/dev/ttyS0", BAUD4800, 5, "IPB/CO");
        //addProxy(&tsPort, "/dev/ttyS1", BAUD9600, 6, "Touchscreen");
    }
#else
    addProxy(&consolePort, "/dev/null", BAUD115200, 0, "Null");
    addProxy(&systemPort, "/dev/zero", BAUD115200, 1, "Zero");
#endif

    statusBar = new QStatusBar(this);
    statusBar->showMessage("Proxy server is starting ...");

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(machineLabel, 0, 0);
    mainLayout->addWidget(machine, 0, 1);
    mainLayout->addWidget(quitButton, 0, 7);
    mainLayout->addWidget(ipAddrLabel, 1, 0);
    mainLayout->addLayout(ipAddrLayout, 1, 1, 1, 2);
    mainLayout->addWidget(applyButton, 1, 3);
    mainLayout->addWidget(resetButton, 1, 7);
    mainLayout->addWidget(tableWidget, 3, 0, 1, 8);
    mainLayout->addWidget(deviceDescription, 4, 0, 1, 8);
    deviceDescription->setHidden(true);
    mainLayout->addWidget(statusBar, 5, 0, 1, 8);
    setLayout(mainLayout);
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
            "Connected" : "Disconnected");
}

void MainWindow::apply()
{
    QString s;
    bool machtypeischanged = false;

    if (machine->currentText() != config->getMachType()) {
        machtypeischanged = true;
        config->setMachType(machine->currentText());
    }

    s.sprintf("%d.%d.%d.%d",
            ipAddr0->value(),
            ipAddr1->value(),
            ipAddr2->value(),
            ipAddr3->value());
    config->setHostAddress(s);

    QStringList maclist = config->getMacEther().split(":");
    maclist[5] = QString::number(ipAddr3->value(), 16);
    config->setMacEther(maclist.join(":"));

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

void MainWindow::quit()
{
    if (QMessageBox::question(this, "", "Quit proxy mode?", QMessageBox::Ok | QMessageBox::Cancel)
            == QMessageBox::Ok) {
        QProcess::execute("rm", QStringList() << "/cfgdir/tty-proxy");
        QProcess::execute("sync");
        QProcess::execute("reboot");
    }
}
