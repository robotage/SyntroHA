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

#ifndef INSTEON_DEVICE_H
#define INSTEON_DEVICE_H

#include <qstring.h>
#include <qdatetime.h>
#include <qstringlist.h>
#include <qjsonobject.h>

//  JSON message defs

#define INSTEON_DEVICE_JSON_UPDATELIST      "updateList"
#define INSTEON_DEVICE_JSON_SETDEVICELEVEL  "setDeviceLevel"

//  JSON field defs

#define INSTEON_DEVICE_JSON_NAME            "name"
#define INSTEON_DEVICE_JSON_FLAGS           "flags"
#define INSTEON_DEVICE_JSON_GROUP           "group"
#define INSTEON_DEVICE_JSON_DEVICEID        "deviceID"
#define INSTEON_DEVICE_JSON_DEVICECAT       "deviceCat"
#define INSTEON_DEVICE_JSON_DEVICESUBCAT    "deviceSubcat"
#define INSTEON_DEVICE_JSON_CURRENTLEVEL    "currentLevel"
#define INSTEON_DEVICE_JSON_NEWLEVEL        "newLevel"


typedef struct
{
    int devCat;                                             // device category
    int devSubcat;                                          // sub category
    QString type;
} DEVICEMAP;

class InsteonDevice
{
public:
	InsteonDevice();
	InsteonDevice(const InsteonDevice &rhs);
	~InsteonDevice();

    bool read(const QJsonObject& json);
    void write(QJsonObject& json);

    bool readNewLevel(const QJsonObject& json);
    void writeNewLevel(QJsonObject& json);

    QString name;
	unsigned char flags;
	unsigned char group;
    unsigned int  deviceID;
	unsigned char deviceCat;
	unsigned char deviceSubcat;

	// transient data
	bool poll;
	QDateTime lastTimeSeen;
    int timeouts;
    int currentLevel;
    int newLevel;

	static QStringList getDeviceTypes();
    static const QString& getTypeFromCat(int devCat, int devSubcat);
	static void getCatFromType(const QString& type, int& cat, int& subcat);
	static QString deviceIDToString(int deviceID);
};

#endif // INSTEON_DEVICE_H
