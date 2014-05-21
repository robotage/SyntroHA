//
//  Copyright (c) 2014 richards-tech.
//	
//  This file is part of SyntroNet
//
//  SyntroNet is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  SyntroNet is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with SyntroNet.  If not, see <http://www.gnu.org/licenses/>.
//

#include <qheaderview.h>
#include <qevent.h>
#include <qdebug.h>
#include <qscrollbar.h>

#include "MainTable.h"
#include "GestureWidget.h"

MainTable::MainTable(QWidget *parent) :
    QTableWidget(parent)
{
    setColumnCount(2);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    verticalScrollBar()->setVisible(false);
    resize(parent->width(), parent->height());
    setColumnWidth(0, width() / 2);
    setColumnWidth(1, width() / 2);
    verticalHeader()->setVisible(false);
    setHorizontalHeaderLabels(QStringList() << tr("Device") << tr("State") );
    setContentsMargins(10, 10, 10, 10);

    m_currentDelta = 0;
    viewport()->grabGesture(Qt::PanGesture);
    viewport()->grabGesture(Qt::SwipeGesture);
    viewport()->grabGesture(Qt::PinchGesture);
    viewport()->grabGesture(Qt::TapGesture);
    viewport()->grabGesture(Qt::TapAndHoldGesture);

 //   setAttribute(Qt::WA_AcceptTouchEvents);
 }


bool MainTable::viewportEvent(QEvent *event)
{
    switch (event->type()) {
    case QEvent::Gesture:
        processGesture((QGestureEvent *)event);
        return true;

    case QEvent::TouchBegin:
        touchBegin((QTouchEvent *)event);
        return false;

    case QEvent::TouchUpdate:
        touchUpdate((QTouchEvent *)event);
        return false;

    case QEvent::TouchEnd:
        touchEnd((QTouchEvent *)event);
        return false;

    case QEvent::TouchCancel:
        touchCancel((QTouchEvent *)event);
        return false;

     default:
        return QAbstractScrollArea::viewportEvent(event);
    }
}


void MainTable::newSize(QSize theSize)
{
    resize(theSize);
    setColumnWidth(0, (theSize.width() - 40) / 2);
    setColumnWidth(1, (theSize.width() - 40) / 2);
}


void MainTable::touchBegin(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> tpList = event->touchPoints();
    if (tpList.count() != 1)
        return;

    qDebug() << "touch begin";
}

void MainTable::touchUpdate(QTouchEvent *event)
{
    int sliderDelta = 0;

    QList<QTouchEvent::TouchPoint> tpList = event->touchPoints();
    if (tpList.count() != 1)
        return;

    qDebug() << "touch update";

    QTouchEvent::TouchPoint tp = tpList.first();
    int delta = tp.pos().y() - tp.lastPos().y();
    m_currentDelta -= delta;
    if (m_currentDelta > MAINTABLE_ROW_HEIGHT) {
        m_currentDelta = 0;
        sliderDelta = 1;
    } else {
        if (m_currentDelta < -MAINTABLE_ROW_HEIGHT) {
            m_currentDelta = 0;
            sliderDelta = -1;
        }
    }
    verticalScrollBar()->setValue(verticalScrollBar()->value() + sliderDelta);
    update();
}

void MainTable::touchEnd(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> tpList = event->touchPoints();
    if (tpList.count() != 1)
        return;

    qDebug() << "touch end";

}

void MainTable::touchCancel(QTouchEvent *event)
{
    QList<QTouchEvent::TouchPoint> tpList = event->touchPoints();
    if (tpList.count() != 1)
        return;

    qDebug() << "touch cancel";

}


bool MainTable::processGesture(QGestureEvent *event)
{
    QGesture *gesture;
    qDebug() << "processGesture";
    if ((gesture = event->gesture(Qt::SwipeGesture)))
        swipe((QSwipeGesture *)gesture);
    else if ((gesture = event->gesture(Qt::PanGesture)))
        pan((QPanGesture *)gesture);
    else if ((gesture = event->gesture(Qt::PinchGesture)))
        pinch((QPinchGesture *)gesture);
    else if ((gesture = event->gesture(Qt::TapGesture)))
        tap((QTapGesture *)gesture);
    else if ((gesture = event->gesture(Qt::TapAndHoldGesture)))
        return tapAndHold((QTapAndHoldGesture *)gesture);
    return true;
}

void MainTable::swipe(QSwipeGesture *gesture)
{
    qDebug() << "swipe " << gesture->swipeAngle();
}

void MainTable::pan(QPanGesture *gesture)
{
   qDebug() << "pan " << gesture->delta();
   verticalScrollBar()->setValue(verticalScrollBar()->value() - gesture->delta().y() / 2);
   update();

}

void MainTable::pinch(QPinchGesture *gesture)
{
   qDebug() << "pinch " << gesture->centerPoint();
}

void MainTable::tap(QTapGesture *gesture)
{
   qDebug() << "tap " << gesture->position();
}

bool MainTable::tapAndHold(QTapAndHoldGesture *gesture)
{
   qDebug() << "tapAndHold " << gesture->gestureType();
   emit tapAndHold();
   return false;
}


