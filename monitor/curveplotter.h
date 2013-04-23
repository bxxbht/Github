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

#ifndef CURVE_PLOTTER_H
#define CURVE_PLOTTER_H

#include <QPen>
#include <QPointer>
#include <QQueue>
#include <QWidget>
#include <QVector>

#include "xfifo.h"

class QImage;

class CurvePlotter : public QWidget
{
	Q_OBJECT

public:

	static bool frozen;

	enum ShiftSpeed {
		ShiftSpeed06p25 = 0x01,
		ShiftSpeed12p50 = 0x02,
		ShiftSpeed25p00 = 0x04,
		ShiftSpeed50p00 = 0x08,
	};

	enum PaintHints {
		PaintNoneHint = 0x00,
		PaintGridHint = 0x01,
		PaintWaveHint = 0x02,
		PaintAllHint = 0xffffffff,
	};

public:
	CurvePlotter(QWidget *parent = 0);
	~CurvePlotter();
	virtual QSize minimumSizeHint() const;
	void setSampleAttrib(quint32 samplesPerSecond, ShiftSpeed speed, qreal scale = 1.0);
	void setRange(const qint16 min, const qint16 max)
	{
		sampleMin = min;
		sampleMax = max;
	}
	void setCurveColor(const QColor &color);
	void setShowTimeStamp(bool on);
	static void setShowGrid(bool on) { showGrid = on; }
	static const bool isShowGrid() { return showGrid; };
	static void setFrozen(bool on) { frozen = on; }
	static const bool isFrozen() { return frozen; };
	inline void append(const qint16 data)
	{
		if (rawData->isFull()) return;
		rawData->enqueue(data);
	}

protected:
	virtual void paintEvent(QPaintEvent *);
	virtual void resizeEvent(QResizeEvent *event);
	virtual bool event(QEvent *event);

private:
	void initGridPen();
	const QPen &gridPen(int line);
	const qreal curveAxisY();

private Q_SLOTS:
	void refresh();

private:
	static const qreal inch2Centimeter = 2.539999918;
	int dpiX;
	int dpiY;
	qreal dotsPerCentimeterX;
	qreal dotsPerCentimeterY;

	quint32 paintHint;

	QImage *background;

	QRect rectGrid;
	QPen gridPen0;
	QPen gridPen1;
	QPen gridPen2;
	static bool showGrid;

	QRect rectTimeStamp;
	qint16 timeStampCount;
	QColor timeStampColor;
	bool showTimeStamp;

	QPen curvePen;

	ShiftSpeed shiftSpeed;
	
	static const qint16 refreshRate = 20.0;
	static const qint16 blankingSpace = 12;
	qreal shiftStepper;
	qint16 indicatorMax;
	qint16 indicatorPre;
	qint16 indicator;
	qreal indicatorF;
	QPointF lastPoint;

	XFiFo<qint16> *rawData;
	quint16 samplesFrames;
	quint16 samplesPerFrame;
	quint32 sampleRate;
	quint16 sampleMin;
	quint16 sampleMax;
	qreal sampleScale;
};

#endif
