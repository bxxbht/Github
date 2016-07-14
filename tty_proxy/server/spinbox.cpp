/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
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

#include <QtGui>
#include <QDebug>

#include "spinbox.h"

SpinBox::SpinBox(QWidget *parent)
    : QSpinBox(parent)
{

}

void SpinBox::keyPressEvent(QKeyEvent *event)
{
    int steps = 1;

    if ((event->key() >= Qt::Key_0) && (event->key() <= Qt::Key_9))
        return;
    if ((event->key() >= Qt::Key_A) && (event->key() <= Qt::Key_Z))
        return;

    switch (event->key()) {
    case Qt::Key_Left:
        focusPreviousChild();
        break;
    case Qt::Key_Right:
        focusNextChild();
        break;
    case Qt::Key_F2:
    case Qt::Key_F4: {
        event->accept();
        const bool up = (event->key() == Qt::Key_F2);
        if (!(stepEnabled() & (up ? StepUpEnabled : StepDownEnabled)))
            return;
        if (!up)
            steps *= -1;
        stepBy(steps);
        return;
    }
    default:
        QSpinBox::keyPressEvent(event);
    }
}