/*
 * A simple patient monitor demo program.
 *
 * Copyright (C) 2011 longfeng.xiao <xlongfeng@126.com>
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

#include <QBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include "nbptrend.h"

const int maxColumns = 6;

NbpTrend::NbpTrend(QWidget *parent)
	: QFrame(parent)
{
	QLabel *label = new QLabel(tr("NBP Trend"));

	QHBoxLayout *hLayout = new QHBoxLayout;
	hLayout->setContentsMargins(8, 4, 0, 0);
	hLayout->insertSpacing(0, 8);

	hLayout->addWidget(label);
	hLayout->addStretch();

	tableWidget = new QTableWidget(0, maxColumns);
	// tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
	tableWidget->verticalHeader()->setVisible(false);
	tableWidget->setAlternatingRowColors(true);
	tableWidget->setSelectionBehavior(QTableWidget::SelectRows);
	tableWidget->setShowGrid(false);
	tableWidget->setHorizontalHeaderLabels(
			QStringList()
			<< tr("Data/Time")
			<< tr("HR(bpm)")
			<< tr("PVC")
			<< tr("SpO2(%)")
			<< tr("PR(bmp)")
			<< tr("NBP(mmHg)")
			<< tr("Status"));
	tableWidget->setColumnWidth(0, 160);
	tableWidget->setColumnWidth(1, 80);
	tableWidget->setColumnWidth(2, 40);
	tableWidget->setColumnWidth(3, 80);
	tableWidget->setColumnWidth(4, 100);
	tableWidget->setColumnWidth(5, 100);

	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->addLayout(hLayout);
	vLayout->addWidget(tableWidget);

	setLayout(vLayout);
}

NbpTrend::~NbpTrend()
{

}

void NbpTrend::addNbpTrend(const QString &date, const QString &hr,
		const QString &pvc, const QString &spo2, const QString &pr, const QString &nbp)
{
	int row = addRow();

	tableWidget->item(row, 0)->setText(date);
	tableWidget->item(row, 1)->setText(hr);
	tableWidget->item(row, 2)->setText(pvc);
	tableWidget->item(row, 3)->setText(spo2);
	tableWidget->item(row, 4)->setText(pr);
	tableWidget->item(row, 5)->setText(nbp);
}

void NbpTrend::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	painter.setPen(QPen(QColor("#3c3a39"), 1));
	painter.drawLine(rect().topLeft(), rect().topRight());

	painter.fillRect(rect().adjusted(0, 1, 0, 0), QBrush("#2b2b27"));
}

int NbpTrend::addRow()
{
        int row = tableWidget->rowCount();

        tableWidget->insertRow(row);

        QTableWidgetItem *item0 = new QTableWidgetItem;
        item0->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget->setItem(row, 0, item0);

        QTableWidgetItem *item1 = new QTableWidgetItem;
        item1->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget->setItem(row, 1, item1);

        QTableWidgetItem *item2 = new QTableWidgetItem;
        item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
        tableWidget->setItem(row, 2, item2);

        QTableWidgetItem *item3 = new QTableWidgetItem;
        item3->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget->setItem(row, 3, item3);

        QTableWidgetItem *item4 = new QTableWidgetItem;
        item4->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget->setItem(row, 4, item4);

        QTableWidgetItem *item5 = new QTableWidgetItem;
        item5->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
        tableWidget->setItem(row, 5, item5);

	return row;
}

