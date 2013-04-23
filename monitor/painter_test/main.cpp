/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  09/22/2011 09:05:04 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */

#include <QApplication>
#include <QtGui>
#include <QDebug>

#include "main.h"

MainWindow::MainWindow(QWidget *parent)
	: QDialog(parent)
{
	resize(400, 300);
}

MainWindow::~MainWindow()
{
}

void MainWindow::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);

	qDebug() << rect();
	QRectF rectF0(20.5, 20.5, 40, 40);

	QRectF rectF1(21.5, 21.5, 38, 38);

	qDebug() << rectF0 << rectF1;

	/* draw box outline */
	painter.setPen(QPen(QColor("#000000"), 1));
	painter.drawRoundedRect(rectF0, 5, 5);

	/* fill background */
	QRectF rectClip2(20.5, 21.5, 39, 37);
	QRectF rectF3(21, 21, 39, 39);
	qDebug() << rectClip2;
	painter.setClipRect(rectClip2);
	QLinearGradient gradient(rectF3.topLeft() + QPointF(0, 2), rectF3.bottomLeft() + QPointF(0, -1));
	gradient.setColorAt(0.0, QColor("#54524f"));
	gradient.setColorAt(1.0, QColor("#373534"));
	painter.setBrush(QBrush(gradient));
	painter.setPen(QPen(Qt::NoPen));
	painter.drawRoundedRect(rectF3, 4, 4);

#if 0
	/* draw top line */
	QRectF rectClip0(20.5, 20.5, 39, 2);
	qDebug() << rectClip0;
	painter.setClipRect(rectClip0);
	painter.setPen(QPen(QColor("#8b8a88"), 1));
	painter.drawRoundedRect(rectF1, 4, 4);

	/* draw bottom line */
	QRectF rectClip1(20.5, 20.5 + 40 - 2 - 1, 39, 2);
	qDebug() << rectClip1;
	painter.setClipRect(rectClip1);
	painter.setPen(QPen(QColor("#2a2926"), 1));
	painter.drawRoundedRect(rectF1, 4, 4);
#endif
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MainWindow window;
	window.show();

	return app.exec();
}
