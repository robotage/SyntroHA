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

cache()

TEMPLATE = app
TARGET = SyntroHAController
DESTDIR = Output

QT += core gui network widgets

CONFIG += mobility

DEFINES += QT_NETWORK_LIB

INCLUDEPATH += GeneratedFiles

MOC_DIR += GeneratedFiles/release

OBJECTS_DIR += release

UI_DIR += GeneratedFiles

RCC_DIR += GeneratedFiles

include(../Src/Src.pri)
include(../../Common/Common.pri)
include(../../../SyntroCore/SyntroLib/SyntroLib.pri)
include(../../../SyntroCore/SyntroGUI/SyntroGUI.pri)

