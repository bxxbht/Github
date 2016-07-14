/*
 * Maya - software update utility
 *
 * Copyright (c) 2015, longfeng.xiao <xlongfeng@126.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

#include <QPaintEngine>
#include <QPaintDevice>
#include <QProgressBar>
#include <QMessageBox>
#include <QTableWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <QProcess>
#include <QDebug>

#include "dialog.h"
#include "ui_dialog.h"
#include "partition.h"
#include "hal.h"

#include <QTreeWidgetItem>

#ifdef __arm__
static const char *upgradeDirPath = "/usb/usbupdate";
static const char *mayaIni = "/cfgdir/.maya.ini";
#else
static const char *upgradeDirPath = "./usbupdate";
static const char *mayaIni = "./maya.ini";
#endif

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Dialog),
    settings(new QSettings(mayaIni, QSettings::IniFormat, parent)),
    updatePartIndex(0),
    updatePartCount(0)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Apply)->setDisabled(true);
    connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(buttonClicked(QAbstractButton*)));
    connect(ui->packageComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(packageChanged(int)));
    QTimer::singleShot(200, this, SLOT(searchPackage()));
}

Dialog::~Dialog()
{
    delete ui;
}

void Dialog::buttonClicked(QAbstractButton *button)
{
    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
        int index = ui->packageComboBox->currentIndex();
        PartitionPackage *package = Partition::package(index);
        if (!package)
            return;
        const QString version = package->version;
        if (QMessageBox::question(this, "", QString("Continue to update '%1'?").arg(version), \
               QMessageBox::Ok | QMessageBox::Cancel) \
            == QMessageBox::Cancel)
            return;

        startUpdate();
    }

    if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
#ifdef __arm__
        QProcess::execute("killall", QStringList() << "syslogd" << "klogd");
        QProcess::execute("sync");
        QProcess::execute("reboot");
#else
        QMessageBox::information(this, "", "Reboot system");
        qApp->quit();
#endif
    }
}

void Dialog::searchPackage()
{
    QDir upgradeDir(upgradeDirPath);
    const QFileInfoList &fileInfoList = upgradeDir.entryInfoList(QDir::AllDirs, QDir::Time);
    QListIterator<QFileInfo> dirList(fileInfoList);
    while (dirList.hasNext()) {
        const QFileInfo &dir = dirList.next();
        const QString &directory = dir.fileName();
        if (directory == "." || directory == "..")
            continue;
        addPackage(dir.absoluteFilePath());
    }
}

void Dialog::addPackage(const QString &directory)
{
    int index = ui->packageComboBox->count();
    PartitionPackage *package = Partition::insertPackage(index, directory);
    if (!package)
        return;

    QTableWidget *tableWidget = new QTableWidget();
    tableWidget->verticalHeader()->setVisible(false);
    tableWidget->setFocusPolicy(Qt::NoFocus);
    tableWidget->setColumnCount(4);
    tableWidget->setHorizontalHeaderLabels(
        QStringList() << "Partition" << "Version" << "Status" << "Progress");
    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setColumnWidth(1, 192);

    ui->packageComboBox->addItem(package->version);

    QListIterator<Partition *> i(package->partitions);
    while (i.hasNext()) {
        insertPartition(tableWidget, i.next());
    }

    ui->stackedWidget->addWidget(tableWidget);
}

void Dialog::insertPartition(QTableWidget *table, Partition *part)
{
    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem *item = new QTableWidgetItem(part->getGroup());
    table->setItem(row, 0, item);

    item = new QTableWidgetItem(part->getNewVersion());
    table->setItem(row, 1, item);

    QLabel *status = new QLabel(part->getErrorDetails());
    table->setCellWidget(row, 2, status);
    connect(part, SIGNAL(statusChanged(QString)), status, SLOT(setText(QString)));

    QProgressBar *progressBar = new QProgressBar();
    connect(part, SIGNAL(progressChanged(int)), progressBar, SLOT(setValue(int)));
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    table->setCellWidget(row, 3, progressBar);
}

void Dialog::packageChanged(int index)
{
    const QString &version = ui->packageComboBox->currentText();
    ui->packageGroupBox->setTitle(version);
    ui->stackedWidget->setCurrentIndex(index);

    QPushButton *button = ui->buttonBox->button(QDialogButtonBox::Apply);
    button->setDisabled(true);

    if (Partition::validatePackage(index))
        button->setEnabled(true);
}

void Dialog::startUpdate()
{
    ui->packageComboBox->setDisabled(true);
    ui->buttonBox->setDisabled(true);

    int index = ui->packageComboBox->currentIndex();
    PartitionPackage *package = Partition::package(index);

    updatePartIndex = 0;
    updatePartCount = package->count();

    QSettings settings(mayaIni, QSettings::IniFormat);
    settings.setValue("magic", "maya");
    settings.setValue("version", package->version);

    nextUpdate();
}

void Dialog::nextUpdate()
{
    int index = ui->packageComboBox->currentIndex();

    if (updatePartIndex == updatePartCount) {
        stopUpdate(Partition::validatePackage(index) == false);
        return;
    }

    PartitionPackage *package = Partition::package(index);
    Partition *part = package->partitions[updatePartIndex];
    connect(part, SIGNAL(finished()), this, SLOT(nextUpdate()));
    package->partitions[updatePartIndex]->start();
    QSettings settings(mayaIni, QSettings::IniFormat);
    settings.beginGroup(part->getGroup());
    settings.setValue("version", part->getNewVersion());
    settings.endGroup();

    updatePartIndex++;
}

void Dialog::stopUpdate(bool failed)
{
    ui->packageLabel->setHidden(true);
    ui->packageComboBox->setHidden(true);
    ui->buttonBox->setStandardButtons(QDialogButtonBox::NoButton);
    ui->buttonBox->addButton("Reboot", QDialogButtonBox::ResetRole);
    ui->buttonBox->setEnabled(true);
    if (failed)
        QMessageBox::warning(this, "", "Update failed!");
    else
        QMessageBox::information(this, "", "Update success!");
}

void Dialog::critical(const QString &text)
{
    QMessageBox::critical(this, "", text);
}
