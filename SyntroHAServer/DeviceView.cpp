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

#include "DeviceView.h"

DeviceView::DeviceView(QWidget *parent)
	: QTableWidget(parent)
{
	layoutWindow();
}

void DeviceView::updateDeviceList(QList<InsteonDevice> deviceList)
{
	int current = currentRow();

	m_deviceList = deviceList;

	if (m_deviceList.count() < rowCount()) {
		setRowCount(m_deviceList.count());
	}

	if (m_deviceList.count() > rowCount()) {
		for (int i = rowCount(); i < m_deviceList.count(); i++)
			addBlankRow();
	}

	for (int i = 0; i < m_deviceList.count(); i++)
		updateDevice(i, m_deviceList[i]);

    if ((current >= 0) && (current < rowCount()))
		selectRow(current);
}

void DeviceView::updateDevice(int row, InsteonDevice device)
{
    QString deviceID;

    setItem(row, 0, new QTableWidgetItem(InsteonDevice::deviceIDToString(device.deviceID)));
    setItem(row, 1, new QTableWidgetItem(device.name));
    setItem(row, 2, new QTableWidgetItem(InsteonDevice::getTypeFromCat(device.deviceCat, device.deviceSubcat)));
    setItem(row, 3, new QTableWidgetItem(QString::number(device.group)));
    setItem(row, 4, new QTableWidgetItem(QString::number(device.flags)));
    setItem(row, 5, new QTableWidgetItem(QString::number(device.currentLevel)));
    setItem(row, 6, new QTableWidgetItem(QString::number(device.timeouts)));
	
	QString s = device.lastTimeSeen.time().toString("HH:mm:ss");
    setItem(row, 7, new QTableWidgetItem(s));
}

InsteonDevice DeviceView::getCurrentDevice()
{
	InsteonDevice device;

	int row = currentRow();

	if (row >= 0 && row < m_deviceList.count())
		device = m_deviceList[row];
/*
	if (row >= 0 && row < rowCount() - 1) {
		device.deviceID = item(row, 0)->text().toInt();	
		device.name = item(row, 1)->text();
		device.deviceCat = item(row, 2)->text().toInt();
		device.deviceSubcat = item(row, 3)->text().toInt();
		device.group = item(row, 4)->text().toInt();
		device.flags = item(row, 5)->text().toInt();
		device.currentLevel = item(row, 6)->text().toInt();
	}
*/
	return device;
}

void DeviceView::mouseDoubleClickEvent(QMouseEvent *)
{
	int row = currentRow();

    if ((row >= 0) && (row < rowCount()))
		emit doubleClickEvent();
}

void DeviceView::layoutWindow()
{
	QStringList headerLabels;

    headerLabels << "Device ID" << "Name" << "Type" << "Group"
                 << "Flags" << "Current Level" << "Timeouts" << "Last Seen";

	verticalHeader()->setDefaultSectionSize(20);
	verticalHeader()->setVisible(false);

	setColumnCount(headerLabels.count());
	setHorizontalHeaderLabels(headerLabels);
	horizontalHeader()->setStretchLastSection(true);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

    setColumnWidth(1, 250);
    setColumnWidth(2, 250);

	addBlankRow();
}

void DeviceView::addBlankRow()
{
	insertRow(0);

	for (int i = 0; i < columnCount(); i++)
		setItem(0, i, new QTableWidgetItem(""));
}
