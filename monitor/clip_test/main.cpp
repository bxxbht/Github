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

	QRect rect0(10, 10, 20, 20);
	painter.setPen(Qt::red);
	// painter.setClipRect(rect0);
	painter.drawRect(rect0);

	QRect rect1(10, 35, 20, 20);
	painter.setPen(Qt::NoPen);
	painter.setClipRect(rect1);
	painter.fillRect(rect1, Qt::red);

	QRect rect2(10, 60, 20, 20);
	painter.setClipRect(rect2);
	painter.fillRect(rect2, Qt::red);

	QRect rect3(10, 85, 20, 20);
	painter.setPen(Qt::blue);
	painter.setClipRect(rect3);
	painter.drawLine(0, 90, 100, 90);
	painter.drawLine(20, 0, 20, 200);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);

	MainWindow window;
	window.show();

	return app.exec();
}
