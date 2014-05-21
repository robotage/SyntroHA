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

#include "InsteonDevice.h"
#include <qjsonvalue.h>

static DEVICEMAP deviceMap[] = {
    {0x00, 0x00, "ControlLinc (2430)"},
    {0x00, 0x04, "RemoteLinc (2440)"},
    {0x00, 0x0a, "Poolux LCD Controller"},
    {0x00, 0x0b, "Access Point (2443)"},
    {0x00, 0x0c, "IES Color Touchscreen (12005)"},
    {0x00, 0x10, "RemoteLinc 2 Keypad 4 scene (2444A2xx4)"},
    {0x00, 0x11, "RemoteLinc 2 switch"},
    {0x00, 0x12, "RemoteLinc 2 keypad 8 scene (2444A2xx8)"},
    {0x00, 0x13, "INSTEON diagnostics keypad (2993-222"},
    {0x00, 0x19, "INSTEON mini-remote (2342-542"},
    {0x00, 0x1d, "Range extender (2992-222)"},

    {0x01, 0x00, "LampLinc 3-pin (2456D3)"},
    {0x01, 0x01, "SwitchLinc dimmer (2476D)"},
    {0x01, 0x05, "Keypad countdown timer with dimmer (2484DWH8)"},
    {0x01, 0x06, "LampLinc dimmer 2 pin (2456D2)"},
    {0x01, 0x07, "ICON LampLinc (2856D2B)"},
    {0x01, 0x09, "KeypadLinc dimmer (2486D)"},
    {0x01, 0x0e, "LampLinc dual band (2457D2)"},
    {0x01, 0x17, "ToggleLinc dimmer (2466D)"},
    {0x01, 0x1a, "In-LineLinc dimmer (2475D)"},
    {0x01, 0x1b, "KeypadLinc dimmer (2486DWH6)"},
    {0x01, 0x1c, "KeypadLinc dimmer (2486DWH6)"},
    {0x01, 0x1d, "SwitchLinc dimmer high wattage (2476DH)"},
    {0x01, 0x1e, "ICON switch dimmer (2476DB)"},

    {0x02, 0x09, "ApplianceLinc 3-pin (2456S3)"},

    {0x10, 0x01, "Motion sensor (2842-222)"},
    {0x10, 0x05, "Motion sensor (2842-522)"},
    {0x10, 0x07, "Open/close sensor (2843-522"},
    {0x10, 0x08, "Leak sensor (2852-222)"},

    // Sentinel

    {0xff, 0xff, "Unknown device"}
};

const QString& InsteonDevice::getTypeFromCat(int devCat, int devSubcat)
{
    int i;

    for (i = 0; deviceMap[i].devCat != 0xff; i++) {
        if ((devCat == deviceMap[i].devCat) && (devSubcat == deviceMap[i].devSubcat))
            return deviceMap[i].type;
    }
    return deviceMap[i].type;
}

QStringList InsteonDevice::getDeviceTypes()
{
	QStringList list;
    for (int i = 0; deviceMap[i].devCat != 0xff; i++) {
		list.append(deviceMap[i].type);
	}
	return list;
}

void InsteonDevice::getCatFromType(const QString& type, int& cat, int& subcat)
{
    for (int i = 0; deviceMap[i].devCat != 0xff; i++) {
		if (deviceMap[i].type == type) {
			cat = deviceMap[i].devCat;
			subcat = deviceMap[i].devSubcat;
			return;
		}
	}
	cat = 0;
	subcat = 0;
}

QString InsteonDevice::deviceIDToString(int deviceID)
{
	QString stringID;

	stringID.sprintf("%02X.%02X.%02X",
            (deviceID >> 16) & 255,
            (deviceID >> 8) & 255,
            (deviceID >> 0) & 255);

	return stringID;
}


InsteonDevice::InsteonDevice()
{
	flags = 0;
	group = 0;
    deviceID = 0;
	deviceCat = 0;
	deviceSubcat = 0;
	poll = false;
    timeouts = 0;
    currentLevel = 0;
    newLevel = 0;
}

InsteonDevice::InsteonDevice(const InsteonDevice &rhs)
{
	name = rhs.name;
	flags = rhs.flags;
	group = rhs.group;
	deviceID = rhs.deviceID;
	deviceCat = rhs.deviceCat;
	deviceSubcat = rhs.deviceSubcat;
	poll = rhs.poll;
	lastTimeSeen = rhs.lastTimeSeen;
	timeouts = rhs.timeouts;
	currentLevel = rhs.currentLevel;
}

InsteonDevice::~InsteonDevice()
{
}

bool InsteonDevice::read(const QJsonObject& json)
{
    if (!json.contains(INSTEON_DEVICE_JSON_NAME))
        return false;
    name = json[INSTEON_DEVICE_JSON_NAME].toString();

    if (!json.contains(INSTEON_DEVICE_JSON_DEVICEID))
        return false;
    deviceID = json[INSTEON_DEVICE_JSON_DEVICEID].toDouble();

    if (json.contains(INSTEON_DEVICE_JSON_FLAGS))
        flags = json[INSTEON_DEVICE_JSON_FLAGS].toDouble();

    if (json.contains(INSTEON_DEVICE_JSON_GROUP))
        group = json[INSTEON_DEVICE_JSON_GROUP].toDouble();

    if (json.contains(INSTEON_DEVICE_JSON_DEVICECAT))
        deviceCat = json[INSTEON_DEVICE_JSON_DEVICECAT].toDouble();

    if (json.contains(INSTEON_DEVICE_JSON_DEVICESUBCAT))
        deviceSubcat = json[INSTEON_DEVICE_JSON_DEVICESUBCAT].toDouble();

    if (json.contains(INSTEON_DEVICE_JSON_CURRENTLEVEL))
        currentLevel = json[INSTEON_DEVICE_JSON_CURRENTLEVEL].toDouble();

    return true;
}


void InsteonDevice::write(QJsonObject& json)
{
    json[INSTEON_DEVICE_JSON_NAME] = name;
    json[INSTEON_DEVICE_JSON_FLAGS] = flags;
    json[INSTEON_DEVICE_JSON_GROUP] = group;
    json[INSTEON_DEVICE_JSON_DEVICEID] = (int)deviceID;
    json[INSTEON_DEVICE_JSON_DEVICECAT] = deviceCat;
    json[INSTEON_DEVICE_JSON_DEVICESUBCAT] = deviceSubcat;
    json[INSTEON_DEVICE_JSON_CURRENTLEVEL] = currentLevel;
}

bool InsteonDevice::readNewLevel(const QJsonObject& json)
{
    if (!json.contains(INSTEON_DEVICE_JSON_DEVICEID))
        return false;
    deviceID = json[INSTEON_DEVICE_JSON_DEVICEID].toDouble();

    if (json.contains(INSTEON_DEVICE_JSON_NEWLEVEL))
        newLevel = json[INSTEON_DEVICE_JSON_NEWLEVEL].toDouble();

    return true;
}

void InsteonDevice::writeNewLevel(QJsonObject& json)
{
    json[INSTEON_DEVICE_JSON_DEVICEID] = (int)deviceID;
    json[INSTEON_DEVICE_JSON_NEWLEVEL] = newLevel;
}
