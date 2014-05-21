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

#ifndef DEVICE_VIEW_H
#define DEVICE_VIEW_H

#include <qtablewidget.h>
#include <qlist.h>

#include "InsteonDevice.h"

class DeviceView : public QTableWidget
{
	Q_OBJECT

public:
	DeviceView(QWidget *parent = NULL);

	void updateDeviceList(QList<InsteonDevice> deviceList);
	InsteonDevice getCurrentDevice();

signals:
	void doubleClickEvent();

protected:
	void mouseDoubleClickEvent(QMouseEvent *);

private:
	void layoutWindow();	
	void addBlankRow();
	void updateDevice(int row, InsteonDevice device);

	QList<InsteonDevice> m_deviceList;
};

#endif // DEVICE_VIEW_H
