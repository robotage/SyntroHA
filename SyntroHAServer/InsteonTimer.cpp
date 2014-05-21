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

#include "InsteonTimer.h"

InsteonTimer::InsteonTimer()
{
    daysOfWeek = 0;
    mode = 0;
    randomMode = false;
    deltaTime = 0;
    armed = false;
}

InsteonTimer::InsteonTimer(const InsteonTimer &rhs)
{
    name = rhs.name;
    time = rhs.time;
    daysOfWeek = rhs.daysOfWeek;
    mode = rhs.mode;
    randomMode = rhs.randomMode;
    deltaTime = rhs.deltaTime;
    armed = rhs.armed;
    triggerTime = rhs.triggerTime;
	devices = rhs.devices;
}

InsteonTimer::~InsteonTimer()
{
}
