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
#include <QDialogButtonBox>
#include <QPainter>
#include <QTabWidget>

#include <QtGui>

#include "label.h"
#include "preferencedialog.h"

Demo0Tab::Demo0Tab(QWidget *parent)
	: QWidget(parent)
{
	QCheckBox *demoMode = new QCheckBox(tr("Demo mode"));

	QComboBox *networkConnectionsCombo = new QComboBox;
	networkConnectionsCombo->addItem("None");
	networkConnectionsCombo->addItem("TCP port");
	networkConnectionsCombo->addItem("Serial ort");

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setLabelAlignment(Qt::AlignLeft);
	formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	formLayout->addRow(tr("Network Connections:"), networkConnectionsCombo);
	formLayout->addRow(tr("System Maintenace:"), new QPushButton(tr("Enter")));
	formLayout->addRow(tr("System Infomation:"), new QPushButton(tr("About...")));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(demoMode);
	mainLayout->addLayout(formLayout);
	mainLayout->addStretch();

	QHBoxLayout *mainHLayout = new QHBoxLayout;
	mainHLayout->addLayout(mainLayout);
	mainHLayout->addStretch();

	QVBoxLayout *mainVLayout = new QVBoxLayout;
	mainVLayout->addLayout(mainHLayout);
	mainVLayout->addStretch();

	setLayout(mainVLayout);
}

Demo2Tab::Demo2Tab(QWidget *parent)
	: QWidget(parent)
{
	QComboBox *nbpSelectMeasurementCombo = new QComboBox;
	nbpSelectMeasurementCombo->addItem("NBP");
	nbpSelectMeasurementCombo->addItem("IBP");

	QHBoxLayout *alarmLimitLabelLayout = new QHBoxLayout;
	alarmLimitLabelLayout->addWidget(new Label("SYS"));
	alarmLimitLabelLayout->addWidget(new Label("DIA"));
	alarmLimitLabelLayout->addWidget(new Label("MAP"));

	QComboBox *upperAlarmLimitSysCombo = new QComboBox;
	upperAlarmLimitSysCombo->addItem("160");
	QComboBox *upperAlarmLimitDiaCombo = new QComboBox;
	upperAlarmLimitDiaCombo->addItem("100");
	QComboBox *upperAlarmLimitMapCombo = new QComboBox;
	upperAlarmLimitMapCombo->addItem("100");

	QHBoxLayout *upperAlarmLimitLabelLayout = new QHBoxLayout;
	upperAlarmLimitLabelLayout->addWidget(upperAlarmLimitSysCombo);
	upperAlarmLimitLabelLayout->addWidget(upperAlarmLimitDiaCombo);
	upperAlarmLimitLabelLayout->addWidget(upperAlarmLimitMapCombo);

	QComboBox *lowerAlarmLimitSysCombo = new QComboBox;
	lowerAlarmLimitSysCombo->addItem("100");
	QComboBox *lowerAlarmLimitDiaCombo = new QComboBox;
	lowerAlarmLimitDiaCombo->addItem("60");
	QComboBox *lowerAlarmLimitMapCombo = new QComboBox;
	lowerAlarmLimitMapCombo->addItem("60");

	QHBoxLayout *lowerAlarmLimitLabelLayout = new QHBoxLayout;
	lowerAlarmLimitLabelLayout->addWidget(lowerAlarmLimitSysCombo);
	lowerAlarmLimitLabelLayout->addWidget(lowerAlarmLimitDiaCombo);
	lowerAlarmLimitLabelLayout->addWidget(lowerAlarmLimitMapCombo);

	QComboBox *bedsideNbpModeCombo = new QComboBox;
	bedsideNbpModeCombo->addItem("Manual");
	bedsideNbpModeCombo->addItem("Auto");
	QHBoxLayout *bedsideNbpLayout = new QHBoxLayout;
	bedsideNbpLayout->addWidget(bedsideNbpModeCombo);
	bedsideNbpLayout->addWidget(new QPushButton(tr("Start")));

	QFormLayout *formLayout = new QFormLayout;
	formLayout->setLabelAlignment(Qt::AlignLeft);
	formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
	formLayout->addRow(tr("Select Measurement:"), nbpSelectMeasurementCombo);
	formLayout->addRow(tr(""), alarmLimitLabelLayout);
	formLayout->addRow(tr("Upper Alarm Limit:"), upperAlarmLimitLabelLayout);
	formLayout->addRow(tr("Lower Alarm Limit:"), lowerAlarmLimitLabelLayout);
	formLayout->addRow(tr("Bedside NBP:"), bedsideNbpLayout);

	QPushButton *alarmSwitch = new QPushButton(QIcon("images/alarm_normal_on.png"), tr(""));

	QHBoxLayout *mainHLayout = new QHBoxLayout;
	mainHLayout->addLayout(formLayout);
	mainHLayout->addWidget(alarmSwitch);
	mainHLayout->addStretch();

	QVBoxLayout *mainVLayout = new QVBoxLayout;
	mainVLayout->addLayout(mainHLayout);
	mainVLayout->addStretch();

	setLayout(mainVLayout);
}

Demo4Tab::Demo4Tab(QWidget *parent)
	: QWidget(parent)
{
	QLabel *topLabel = new QLabel(tr("Open with:"));

	QListWidget *applicationsListBox = new QListWidget;
	applicationsListBox->setAlternatingRowColors(true);
	QStringList applications;

	for (int i = 1; i <= 300; ++i)
		applications.append(tr("Application %1").arg(i));
	applicationsListBox->insertItems(0, applications);

	QCheckBox *alwaysCheckBox;

	alwaysCheckBox = new QCheckBox(tr("Always use this application to "
					"open this type of file"));

	QVBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(topLabel);
	layout->addWidget(applicationsListBox);
	layout->addWidget(alwaysCheckBox);
	setLayout(layout);
}

PreferenceDialog::PreferenceDialog(QWidget *parent)
	: QDialog(parent)
{
	setMinimumSize(600, 400);
	tabWidget = new QTabWidget;

	tabWidget->addTab(new Demo0Tab, tr("General"));
	tabWidget->addTab(new Demo0Tab, tr("Setup"));
	tabWidget->addTab(new Demo2Tab, tr("NBP Menu"));
	tabWidget->addTab(new Demo0Tab, tr("Channel"));
	tabWidget->addTab(new Demo4Tab, tr("User Info"));

	buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok);
	// connect(buttonBox, SIGNAL(accepted()), this, SLOT(hide()));

	QVBoxLayout *mainLayout = new QVBoxLayout;
	mainLayout->addWidget(tabWidget);
	mainLayout->addWidget(buttonBox);
	setLayout(mainLayout);
}

void PreferenceDialog::paintEvent(QPaintEvent *)
{
	QPainter painter(this);

	painter.setPen(QPen(QColor("#3c3a39"), 1));
	painter.drawLine(rect().topLeft(), rect().topRight());

	painter.fillRect(rect().adjusted(0, 1, 0, 0), QBrush("#2b2b27"));
}
