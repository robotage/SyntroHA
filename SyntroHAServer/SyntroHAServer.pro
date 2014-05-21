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

TEMPLATE = app

TARGET = SyntroHAServer

win32* {
	DESTDIR = Release
}
else {
	DESTDIR = Output
}

QT += core gui network serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += debug_and_release

unix {
	CONFIG += link_pkgconfig
	macx:CONFIG -= app_bundle
	PKGCONFIG += syntro

	target.path = /usr/bin
	INSTALLS += target
}

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles \
    GeneratedFiles/Release \
    Database

win32-g++:LIBS += -L"$(SYNTRODIR)/bin"

win32-msvc*:LIBS += -L"$(SYNTRODIR)/lib"

win32 {
	DEFINES += _CRT_SECURE_NO_WARNINGS
	INCLUDEPATH += $(SYNTRODIR)/include
	LIBS += -lSyntroLib
}

MOC_DIR += GeneratedFiles/Release

OBJECTS_DIR += Release

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(SyntroHAServer.pri)
include(../Common/Common.pri)
