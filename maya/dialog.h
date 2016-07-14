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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QThread>
#include <QMultiMap>


namespace Ui {
class Dialog;
}

class QSettings;
class QTableWidget;
class QAbstractButton;
class QSettings;
class Partition;

class Dialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit Dialog(QWidget *parent = 0);
    ~Dialog();

public slots:
    void buttonClicked(QAbstractButton *button);
    void searchPackage();
    void insertPartition(QTableWidget *table, Partition *part);
    void packageChanged(int index);
    void startUpdate();
    void nextUpdate();
    void stopUpdate(bool failed);
    void critical(const QString &text);

private:
    void addPackage(const QString &directory);
    
private:
    Ui::Dialog *ui;
    QSettings *settings;

    int updatePartIndex;
    int updatePartCount;
};

#endif // DIALOG_H
