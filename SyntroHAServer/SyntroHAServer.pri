#//
#//  Copyright (c) 2014 richards-tech
#//	
#//  This file is part of SyntroNet
#//
#//  SyntroNet is free software: you can redistribute it and/or modify
#//  it under the terms of the GNU General Public License as published by
#//  the Free Software Foundation, either version 3 of the License, or
#//  (at your option) any later version.
#//
#//  SyntroNet is distributed in the hope that it will be useful,
#//  but WITHOUT ANY WARRANTY; without even the implied warranty of
#//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#//  GNU General Public License for more details.
#//
#//  You should have received a copy of the GNU General Public License
#//  along with SyntroNet.  If not, see <http://www.gnu.org/licenses/>.
#//

INCLUDEPATH += $$PWD \
        $$PWD/Common

DEPENDPATH += $$PWD

HEADERS += AddEditInsteonDeviceDlg.h \
	AddEditInsteonTimerDlg.h \
	DeviceView.h \
	InsteonDefs.h \
	InsteonDriver.h \
	InsteonTimer.h \
	InsteonTimerDevice.h \
        HAClient.h \
	SetInsteonLevelDlg.h \
        SyntroHAServer.h \
        SyntroHAWindow.h \
        TimerView.h \
        SerialDlg.h \
        AddDeviceToTimerDlg.h

SOURCES += AddEditInsteonDeviceDlg.cpp \
	AddEditInsteonTimerDlg.cpp \
	DeviceView.cpp \
	InsteonDriver.cpp \
	InsteonTimer.cpp \
	InsteonTimerDevice.cpp \
	main.cpp \
        HAClient.cpp \
	SetInsteonLevelDlg.cpp \
        SyntroHAServer.cpp \
        SyntroHAWindow.cpp \
        TimerView.cpp \
        SerialDlg.cpp \
        AddDeviceToTimerDlg.cpp

FORMS += SyntroHAServer.ui

