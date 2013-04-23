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

#include <QPainter>
#include <QPaintEvent>
#include <QTimer>
#include <QX11Info>
#include <QDebug>
#include "curveplotter.h"

static const QSize minSize(128, 48);
static const QSize gridSize(16, 16);
static const QSize timeStampSize(3, 3);

bool CurvePlotter::showGrid = true;
bool CurvePlotter::frozen = false;

CurvePlotter::CurvePlotter(QWidget *parent)
	: QWidget(parent)
{
	setAttribute(Qt::WA_OpaquePaintEvent, true);

	dpiX = QX11Info::appDpiX();
	dpiY = QX11Info::appDpiY();
	qDebug() << dpiX << dpiX;
	dotsPerCentimeterX = (qreal)dpiX / inch2Centimeter;
	dotsPerCentimeterY = (qreal)dpiY / inch2Centimeter;

	paintHint = PaintAllHint;
	background = new QImage;
	initGridPen();
	showGrid = true;

	timeStampCount = 0;
	timeStampColor = QColor("#ffff00");
	showTimeStamp = false;

	rawData = new XFiFo<qint16>(2048);
	sampleRate = 500000;
	sampleMin = 0;
	sampleMax = 0xffff;

	frozen = 0;

	curvePen.setColor(Qt::green);
	curvePen.setWidth(1);
	indicatorPre = 0;
	indicator = 0;
	indicatorF = 0.0;

	QTimer *refleshTimer = new QTimer(this);
	connect(refleshTimer, SIGNAL(timeout()), this, SLOT(refresh()));
	refleshTimer->start(1000.0 / refreshRate);
}

CurvePlotter::~CurvePlotter()
{
	if (background) {
		delete background;
		background = NULL;
	}
}

QSize CurvePlotter::minimumSizeHint() const
{
	return QSize(128, 64);
}

void CurvePlotter::initGridPen()
{
	QVector<qreal> dashes;

	dashes.clear();
	dashes << 1 << 1;
	gridPen0.setDashPattern(dashes);
	gridPen0.setColor(QColor("#282828"));

	dashes.clear();
	dashes << 1 << 3;
	gridPen1.setDashPattern(dashes);
	gridPen1.setColor(QColor("#282828"));

	dashes.clear();
	dashes << 1 << 15;
	gridPen2.setDashPattern(dashes);
	gridPen2.setColor(QColor("#282828"));
}

const QPen &CurvePlotter::gridPen(int line)
{
	line %= 16;
	switch (line) {
	case 0:
		return gridPen0;
	case 4: case 8: case 12:
		return gridPen1;
	default:
		return gridPen2;
	}
}

void CurvePlotter::setCurveColor(const QColor &color)
{
	curvePen.setColor(color);
}

void CurvePlotter::setShowTimeStamp(bool on)
{
	showTimeStamp = on;
}

const qreal CurvePlotter::curveAxisY()
{
	qreal y;

	if (rawData->isEmpty())
		y = 0.0;
	else {
		y = (qreal)rawData->dequeue();
		if (y > sampleMax) y = sampleMax;
		else if (y < sampleMin) y = sampleMin;
		y -= ((sampleMin + sampleMax) / 2.0);
	}
	y *= sampleScale;
	y += (height() / 2);

	return y;
}

void CurvePlotter::paintEvent(QPaintEvent *event)
{
	if (!background) {
		QPainter painter(this);
		painter.fillRect(rect(), Qt::black);
		return;
	}

	if ((paintHint & PaintGridHint) & showGrid) {
		paintHint &= ~PaintGridHint;
		QPainter painterBackground(background);
		painterBackground.fillRect(rect(), Qt::black);
		for (int i = 0; i < rectGrid.height(); i+=2) {
			painterBackground.setPen(gridPen(i));
			painterBackground.drawLine(rectGrid.x(), 
					rectGrid.y() + i,
					rectGrid.x() + rectGrid.width(), 
					rectGrid.y() + i);
		}
	}

	if (frozen)
		return;

	QPainter painter(this);

	const QRegion &region = event->region();
	const int &rectCount = region.rectCount();
	const QVector<QRect> &rects = region.rects();
	for (int i = 0; i < rectCount; i++) {
		painter.drawImage(rects[i], *background, rects[i]);
	}

	int delta;
	if (indicator >= indicatorPre)
		delta = indicator - indicatorPre;
	else
		delta = indicatorMax - indicatorPre + indicator;

	if (delta == 0)
		return;

	quint16 dots = samplesFrames;
	samplesFrames = 0;
	qreal forwardStrokePerDot = (qreal)delta / dots;

	qreal x;
	qreal y;
	QPainterPath curvePath;
	QPointF to = lastPoint;
	curvePath.moveTo(lastPoint);

	x = indicatorPre;
	for (int i = 0; i < dots; i++) {
		x += forwardStrokePerDot;
		y = curveAxisY();
		if (x > indicatorMax) {
			x -= indicatorMax;
			to.setX(0.0);
			curvePath.moveTo(to);
		}
		to.setX(x);
		to.setY(y);
		curvePath.lineTo(to);
	}

	indicatorPre = indicator;
	lastPoint.setX(to.x());
	lastPoint.setY(to.y());

	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.strokePath(curvePath, curvePen);

	if ((timeStampCount >= refreshRate) && showTimeStamp) {
		timeStampCount = 0;
		painter.setRenderHint(QPainter::Antialiasing, false);
		if (rectTimeStamp.isValid())
			painter.fillRect(rectTimeStamp, timeStampColor);
	}
}

bool CurvePlotter::event(QEvent *event)
{
	switch (event->type()) {
	case QEvent::WindowActivate:
	case QEvent::WindowDeactivate:
		return 0;
		break;
	}
	return QWidget::event(event);
}

void CurvePlotter::resizeEvent(QResizeEvent *event)
{
	QSize oldSize = event->oldSize();
	QSize size = event->size();
	QSize delta = size - minSize;

	if (!size.isEmpty() && !delta.isEmpty()) {
		if (background) {
			delete background;
			background = NULL;
		}
		background = new QImage(size, QImage::Format_RGB32);

		{
			int hSpace, vSpace;
			int hMultiple, vMultiple;
			int hRemainder, vRemainder;

			hMultiple = size.width() / gridSize.width();
			vMultiple = size.height() / gridSize.height();
			hRemainder = size.width() % gridSize.width();
			vRemainder = size.height() % gridSize.height();

			if (hRemainder < 2) {
				hMultiple -= 1;
				hRemainder += gridSize.width();
			}

			if (vRemainder < 2) {
				vMultiple -= 1;
				vRemainder += gridSize.height();
			}

			vRemainder = (vRemainder == 0) ? gridSize.width() : vRemainder;
			rectGrid.setRect(hRemainder / 2, vRemainder / 2,
					hMultiple * gridSize.width(),
					vMultiple * gridSize.height() + 1);

			paintHint |= PaintGridHint;
		}

		{
			lastPoint.setX(0.0);
			lastPoint.setY(size.height() / 2);
			indicatorPre = 0;
			indicator = 0;
			indicatorF = 0.0;
			indicatorMax = size.width();

		}

		{
			samplesFrames = 0;
		}
	} else {
		if (background) {
			delete background;
			background = NULL;
		}
	}
}

void CurvePlotter::setSampleAttrib(quint32 samplesPerSecond, ShiftSpeed speed, qreal scale)
{
	sampleRate = samplesPerSecond;
	samplesPerFrame = sampleRate / refreshRate;
	shiftSpeed = speed;
	sampleScale = scale;

	switch (shiftSpeed) {
	case ShiftSpeed06p25:
		shiftStepper = (.625 * (dpiX / inch2Centimeter)) / refreshRate;
		break;
	case ShiftSpeed12p50:
		shiftStepper = (1.25 * (dpiX / inch2Centimeter)) / refreshRate;
		break;
	case ShiftSpeed25p00:
		shiftStepper = (2.5 * (dpiX / inch2Centimeter)) / refreshRate;
		break;
	case ShiftSpeed50p00:
		shiftStepper = (5.0 * (dpiX / inch2Centimeter)) / refreshRate;
		break;
	}
}

void CurvePlotter::refresh()
{
	if (frozen) {
		for (int i = 0; i < samplesPerFrame; i++)
			curveAxisY();
		return;
	}

	timeStampCount++;
	samplesFrames += samplesPerFrame;
	indicatorF += shiftStepper;
	if (indicatorF >= indicatorMax)
		indicatorF -= indicatorMax;
	indicator = indicatorF;

	QRegion clipRegion;
	int delta;
	if (indicator > indicatorPre)
		delta = indicator - indicatorPre;
	else
		delta = indicatorMax - indicatorPre + indicator;
	if ((indicatorPre + delta + blankingSpace) > indicatorMax) {
		clipRegion += QRect(indicatorPre, 0, indicatorMax - indicatorPre, height());
		clipRegion += QRect(0, 0, (indicatorPre + blankingSpace + delta) - indicatorMax, height());
	} else {
		clipRegion += QRect(indicatorPre, 0, delta + blankingSpace, height());
	}

	QRegion intersectedRegion;
	if ((indicatorPre + blankingSpace) > indicatorMax) {
		intersectedRegion += QRect(indicatorPre, 0, indicatorMax - indicatorPre, height());
		intersectedRegion += QRect(0, 0, (indicatorPre + blankingSpace) - indicatorMax, height());
	} else {
		intersectedRegion += QRect(indicatorPre, 0, blankingSpace, height());
	}

	if ((indicator + blankingSpace) > indicatorMax) {
		intersectedRegion &= QRect(indicator, 0, indicatorMax - indicatorPre, height());
		intersectedRegion &= QRect(0, 0, (indicator + blankingSpace) - indicatorMax, height());
	} else {
		intersectedRegion &= QRect(indicator, 0, blankingSpace, height());
	}
	clipRegion -= intersectedRegion;

	if ((timeStampCount >= refreshRate) && showTimeStamp) {
		rectTimeStamp.setRect(indicator - timeStampSize.width(),
				height() -  timeStampSize.height() - gridSize.height() / 2,
				timeStampSize.width(), timeStampSize.height());
		clipRegion += rectTimeStamp;
	}

	update(clipRegion);
}
