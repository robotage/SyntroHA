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

#ifndef MAINTABLE_H
#define MAINTABLE_H

#include <QTableWidget>
#include <QGestureEvent>

#define MAINTABLE_ROW_HEIGHT 70

class MainTable : public QTableWidget
{
    Q_OBJECT
public:
    explicit MainTable(QWidget *parent = 0);

signals:
    void tapAndHold();

public slots:
    void newSize(QSize);

protected:

    bool viewportEvent(QEvent *event);

private:
    void touchBegin(QTouchEvent *event);
    void touchUpdate(QTouchEvent *event);
    void touchEnd(QTouchEvent *event);
    void touchCancel(QTouchEvent *event);
    bool processGesture(QGestureEvent *event);
    void swipe(QSwipeGesture *gesture);
    void pan(QPanGesture *gesture);
    void pinch(QPinchGesture *gesture);
    void tap(QTapGesture *gesture);
    bool tapAndHold(QTapAndHoldGesture *gesture);

    int m_currentDelta;
 };

#endif // MAINTABLE_H
