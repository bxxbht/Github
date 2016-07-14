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

#include <QDialog>
#include <QTableWidget>

#include "configuration.h"
#include "communication.h"

QT_BEGIN_NAMESPACE
class QComboBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class SpinBox;
class QStatusBar;
class QTableWidget;
QT_END_NAMESPACE

class ComboBox;
class SpinBox;

class MainWindow : public QDialog
{
    Q_OBJECT

public:
    MainWindow(Configuration *config, QWidget *parent = 0);
    ~MainWindow();
    void ipAddrLayout(SpinBox* ipAddr3);

protected:
    void addRow();
    void addProxy(Communication **pport, const char *device,
            BaudRateType bandrate, quint16 offset, const char *desc = 0);
    void loadStyleSheet(const QString &sheetName);

private Q_SLOTS:
    void updateContent(Communication *port, int row);
    void apply();

private:
    Configuration *config;
    ComboBox *machine;
#ifndef __arm__
    QPushButton *quitButton;
#endif
    QPushButton *applyButton;
    QPushButton *resetButton;
    SpinBox *ipAddr0;
    SpinBox *ipAddr1;
    SpinBox *ipAddr2;
    SpinBox *ipAddr3;
    SpinBox *macAddr0;
    SpinBox *macAddr1;
    SpinBox *macAddr2;
    SpinBox *macAddr3;
    SpinBox *macAddr4;
    SpinBox *macAddr5;

    Communication *consolePort;
    Communication *module0Port;
    Communication *module1Port;
    Communication *module2Port;
    Communication *module3Port;
    Communication *printerPort;
    Communication *systemPort;
    Communication *tsPort;

    QTableWidget *tableWidget;
    QTableWidget *deviceDescription;
    QStatusBar *statusBar;
};

#endif