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

#include <QDebug>
#include <QPaintEvent>
#include <QBoxLayout>
#include <QTimer>

#include "ecgchannel.h"
#include "nbptrend.h"
#include "channelpanel.h"

static const qint16 ecg[] = {
	31,31,31,31,31,31,31,31,31,30,30,31,31,30,30,31,31,32,33,33,30,24,18,12, 6,
	 3, 4, 9,16,23,29,30,30,30,30,30,30,29,29,29,29,29,28,28,28,28,28,27,27,26,
	26,26,25,25,25,25,25,25,25,25,26,26,26,27,28,28,29,30,30,31,31,31,31,31,31,
	31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,30,30,29,29,28,28,28,27,27,27,
	27,27,28,28,28,29,29,29,30,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
	31,31,31,31,31,31,31,31,31,30,30,31,31,30,30,31,31,32,33,33,30,24,18,12, 6,
	 3, 4, 9,16,23,29,30,30,30,30,30,30,29,29,29,29,29,28,28,28,28,28,27,27,26,
	26,26,25,25,25,25,25,25,25,25,26,26,26,27,28,28,29,30,30,31,31,31,31,31,31,
	31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,30,30,29,29,28,28,28,27,27,27,
	27,27,28,28,28,29,29,29,30,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,31,
};

ChannelPanel::ChannelPanel(QWidget *parent)
	: QFrame(parent)
{
	ecgChannel1 = new EcgChannel("ECG1");
	ecgChannel1->setLead("I");
	ecgChannel1->setGain("X1");
	ecgChannel1->setSpeed("50 mm/s");
	ecgChannel1->setMode("Monitor");
	ecgChannel1->setColor(Qt::green);
	ecgChannel1->setShowTimeStamp(true);
	ecgChannel1->setSampleAttrib(250, CurvePlotter::ShiftSpeed50p00, 1.0);
	ecgChannel1->setRange(0, 36);

	ecgChannel2 = new EcgChannel("ECG2");
	ecgChannel2->setLead("II");
	ecgChannel2->setGain("X1");
	ecgChannel2->setSpeed("25 mm/s");
	// ecgChannel2->setMode("Monitor");
	ecgChannel2->setColor(Qt::red);
	ecgChannel2->setSampleAttrib(250, CurvePlotter::ShiftSpeed25p00, 1.0);
	ecgChannel2->setRange(0, 36);

	ecgChannel3 = new EcgChannel("ECG3");
	ecgChannel3->setLead("III");
	ecgChannel3->setGain("X1");
	ecgChannel3->setSpeed("12.5 mm/s");
	ecgChannel3->setColor(Qt::blue);
	ecgChannel3->setSampleAttrib(250, CurvePlotter::ShiftSpeed12p50, 1.0);
	ecgChannel3->setRange(0, 36);

	ecgChannel4 = new EcgChannel("ECG4");
	ecgChannel4->setLead("IV");
	ecgChannel4->setGain("X1");
	ecgChannel4->setSpeed("6.25 mm/s");
	ecgChannel4->setColor(Qt::yellow);
	ecgChannel4->setSampleAttrib(250, CurvePlotter::ShiftSpeed06p25, 1.0);
	ecgChannel4->setRange(0, 36);

#if 0
	ecgChannel5 = new EcgChannel("ECG5");
	ecgChannel5->setLead("V");
	ecgChannel5->setGain("X1");
	ecgChannel5->setSpeed("25 mm/s");
	ecgChannel5->setColor(Qt::darkGreen);
	ecgChannel5->setSampleAttrib(250, CurvePlotter::ShiftSpeed25p00, 1.0);
	ecgChannel5->setRange(0, 36);

	ecgChannel6 = new EcgChannel("ECG6");
	ecgChannel6->setLead("VI");
	ecgChannel6->setGain("X1");
	ecgChannel6->setSpeed("25 mm/s");
	ecgChannel6->setColor(Qt::red);
	ecgChannel6->setSampleAttrib(250, CurvePlotter::ShiftSpeed25p00, 1.0);
	ecgChannel6->setRange(0, 36);

	ecgChannel7 = new EcgChannel("ECG7");
	ecgChannel7->setLead("VII");
	ecgChannel7->setGain("X1");
	ecgChannel7->setSpeed("25 mm/s");
	ecgChannel7->setColor(Qt::red);
	ecgChannel7->setSampleAttrib(250, CurvePlotter::ShiftSpeed25p00, 1.0);
	ecgChannel7->setRange(0, 36);

	ecgChannel8 = new EcgChannel("ECG8");
	ecgChannel8->setLead("VIII");
	ecgChannel8->setGain("X1");
	ecgChannel8->setSpeed("25 mm/s");
	ecgChannel8->setColor(Qt::red);
	ecgChannel8->setSampleAttrib(250, CurvePlotter::ShiftSpeed25p00, 1.0);
	ecgChannel8->setRange(0, 36);
#endif

	NbpTrend *nbpTrend = new NbpTrend;
	nbpTrend->addNbpTrend("10/29/2011 13:16:37", "80", "0", "98", "80 SpO2", "122/83(98)");
	nbpTrend->addNbpTrend("10/29/2011 13:18:21", "80", "0", "97", "80 SpO2", "124/82(97)");
	nbpTrend->addNbpTrend("10/29/2011 13:20:43", "80", "0", "99", "80 SpO2", "132/88(101)");
	nbpTrend->addNbpTrend("10/29/2011 13:31:15", "80", "0", "98", "80 SpO2", "135/86(102)");
	nbpTrend->addNbpTrend("10/29/2011 13:52:32", "80", "0", "97", "80 SpO2", "127/82(99)");
	nbpTrend->addNbpTrend("10/29/2011 14:17:28", "80", "0", "98", "80 SpO2", "132/81(97)");
	nbpTrend->addNbpTrend("10/30/2011 13:16:37", "80", "0", "98", "80 SpO2", "122/83(98)");
	nbpTrend->addNbpTrend("10/30/2011 13:18:21", "80", "0", "97", "80 SpO2", "124/82(97)");
	nbpTrend->addNbpTrend("10/30/2011 13:20:43", "80", "0", "99", "80 SpO2", "132/88(101)");
	nbpTrend->addNbpTrend("10/30/2011 13:31:15", "80", "0", "98", "80 SpO2", "135/86(102)");
	nbpTrend->addNbpTrend("10/30/2011 13:52:32", "80", "0", "97", "80 SpO2", "127/82(99)");
	nbpTrend->addNbpTrend("10/30/2011 14:17:28", "80", "0", "98", "80 SpO2", "132/81(97)");

	QVBoxLayout *vLayout = new QVBoxLayout;
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(0);

	vLayout->addWidget(ecgChannel1);
	vLayout->addWidget(ecgChannel2);
	vLayout->addWidget(ecgChannel3);
	vLayout->addWidget(ecgChannel4);
#if 0
	vLayout->addWidget(ecgChannel5);
	vLayout->addWidget(ecgChannel6);
	vLayout->addWidget(ecgChannel7);
	vLayout->addWidget(ecgChannel8);
	vLayout->addStretch();
#endif

	vLayout->addWidget(nbpTrend);

	setLayout(vLayout);

	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(demo()));
	timer->start(1000);
}

ChannelPanel::~ChannelPanel()
{
}

void ChannelPanel::paintEvent(QPaintEvent *event)
{
	QFrame::paintEvent(event);
}

void ChannelPanel::demo()
{
	for (int i = 0; i < 250; i++) {
		ecgChannel1->append(ecg[i]);
		ecgChannel2->append(ecg[i]);
		ecgChannel3->append(ecg[i]);
		ecgChannel4->append(ecg[i]);
#if 0
		ecgChannel5->append(ecg[i]);
		ecgChannel6->append(ecg[i]);
		ecgChannel7->append(ecg[i]);
		ecgChannel8->append(ecg[i]);
#endif
	}
}
