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

#ifndef INSTEON_TIMER_H
#define INSTEON_TIMER_H

#include <qstring.h>
#include <qdatetime.h>

#include "InsteonTimerDevice.h"

class InsteonTimer
{
public:
	InsteonTimer();
	InsteonTimer(const InsteonTimer &rhs);
	~InsteonTimer();

    QString name;                                       // name of timer
    QTime time;                                         // trigger time
    unsigned char daysOfWeek;                           // flags for active on which days of week
    int mode;                                           // if using TOD, sunset or sunrise
    bool randomMode;                                    // if random mode enabled
    int deltaTime;                                      // delta time in minutes
    QList<InsteonTimerDevice> devices;					// devices being controlled
    bool armed;                                         // it has armed (ie not fired so far today)
    QTime triggerTime;                                  // today's calculated time
};

#endif // INSTEON_TIMER_H
