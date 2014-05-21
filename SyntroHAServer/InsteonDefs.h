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

#ifndef INSTEONDEFS_H
#define INSTEONDEFS_H

#include <qdatetime.h>

typedef struct
{
	unsigned int deviceID;
	int category;
	int subCategory;
	int firmwareRev;
} INSTEON_PLM;


//  Command types for the INSTEON_REQUEST structure

//  Timer mode defs

#define INSTEON_TIMER_MODE_TOD              0               // used time of day to trigger
#define INSTEON_TIMER_MODE_SUNRISE          1               // use sunrise to trigger
#define INSTEON_TIMER_MODE_SUNSET           2               // use sunset to trigger


#define INSTEON_REQUEST_TYPE_DIRECT     0               // direct message
#define INSTEON_REQUEST_TYPE_ADD_DEVICE 1               // add a device to the PLM
#define INSTEON_REQUEST_TYPE_REMOVE_DEVICE 2            // remove a device from the PLM

typedef struct
{
    int deviceID;                                       // Insteon device ID of device
    unsigned char requestType;                          // the request type
    unsigned char cmd1;                                 // context dependent device command
    unsigned char cmd2;                                 // context dependent device command

} INSTEON_REQUEST;

#endif // INSTEONDEFS_H
