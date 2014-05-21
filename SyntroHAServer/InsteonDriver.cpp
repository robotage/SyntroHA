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

#include "InsteonDriver.h"
#include <QStringList>
#include <qsettings.h>
#include <qdatetime.h>
#include <qjsonarray.h>
#include <math.h>


//	Settings file keys

#define INSTEON_SETTINGS_PLM                "PLM"

#define INSTEON_SETTINGS_PLM_PORT           "PLMPort"
#define INSTEON_SETTINGS_PLM_LATITUDE       "PLMLatitude"
#define INSTEON_SETTINGS_PLM_LONGITUDE      "PLMLongitude"
#define	INSTEON_SETTINGS_PLM_ID				"PLMId"			
#define	INSTEON_SETTINGS_DEVICE_CATEGORY	"PLMCategory"
#define	INSTEON_SETTINGS_DEVICE_SUBCATEGORY	"PLMSubCategory"
#define	INSTEON_SETTINGS_FIRMWARE_REV		"PLMFirmwareRev"

#define INSTEON_SETTINGS_DEVICE_DATABASE    "DeviceDatabase"
#define INSTEON_SETTINGS_DEVICE_ID          "DeviceID"
#define INSTEON_SETTINGS_DEVICE_NAME        "DeviceName"
#define INSTEON_SETTINGS_DEVICE_FLAGS       "DeviceFlags"
#define INSTEON_SETTINGS_DEVICE_GROUP       "DeviceGroup"
#define INSTEON_SETTINGS_DEVICE_CAT			"DeviceCat"
#define INSTEON_SETTINGS_DEVICE_SUBCAT		"DeviceSubcat"

#define INSTEON_SETTINGS_TIMERS             "Timers"
#define INSTEON_SETTINGS_TIMERS_NAME        "TimerName"
#define INSTEON_SETTINGS_TIMERS_TIMEHOUR    "TimerHour"
#define INSTEON_SETTINGS_TIMERS_TIMEMINUTE  "TimerMinute"
#define INSTEON_SETTINGS_TIMERS_DOW         "TimerDoW"
#define INSTEON_SETTINGS_TIMERS_RANDOM      "TimerRandom"
#define INSTEON_SETTINGS_TIMERS_MODE        "TimerMode"
#define INSTEON_SETTINGS_TIMERS_DELTATIME   "TimerDeltaTime"
#define INSTEON_SETTINGS_TIMERS_DEVICES     "TimerDevices"
#define INSTEON_SETTINGS_TIMERS_DEVICES_ID  "TimerDevicesID"
#define INSTEON_SETTINGS_TIMERS_DEVICES_LEVEL "TimerDevicesLevel"


//  ACK/NAK

#define INSTEON_ACK                         0x06
#define INSTEON_NAK                         0x15

//	Receive only message types

#define	INSTEON_LOWEST_COMMAND				0x50

#define	INSTEON_STANDARD_MESSAGE_RECEIVED	0x50
#define	INSTEON_EXTENDED_MESSAGE_RECEIVED	0x51
#define	INSTEON_X10_RECEIVED				0x52
#define INSTEON_ALL_LINKING_COMPLETED		0x53
#define	INSTEON_BUTTON_EVENT_REPORT			0x54
#define	INSTEON_USER_RESET_DETECTED			0x55
#define	INSTEON_ALL_LINK_CLEANUP_FAILURE	0x56
#define	INSTEON_ALL_LINK_RECORD_RESPONSE	0x57
#define	INSTEON_ALL_LINK_CLEANUP_STATUS		0x58

//	Send and response message types

#define	INSTEON_GET_IM_INFO					0x60
#define	INSTEON_SEND_ALL_LINK_COMMAND		0x61
#define	INSTEON_SEND_MESSAGE				0x62
#define	INSTEON_SEND_X10					0x63
#define	INSTEON_START_ALL_LINKING			0x64
#define	INSTEON_CANCEL_ALL_LINKING			0x65
#define	INSTEON_SET_HOST_CATEGORY			0x66
#define	INSTEON_RESET_IM					0x67
#define	INSTEON_SET_ACK_BYTE				0x68
#define	INSTEON_GET_FIRST_ALL_LINK_RECORD	0x69
#define	INSTEON_GET_NEXT_ALL_LINK_RECORD	0x6A
#define	INSTEON_SET_IM_CONFIG				0x6B
#define	INSTEON_ALL_LINK_RECORD_FOR_SENDER	0x6C
#define	INSTEON_LED_ON						0x6D
#define	INSTEON_LED_OFF						0x6E
#define	INSTEON_MANAGE_ALL_LINK_RECORD		0x6F
#define	INSTEON_SET_NAK_BYTE				0x70
#define	INSTEON_SET_ACK_TWO_BYTES			0x71
#define	INSTEON_RF_SLEEP					0x72
#define	INSTEON_GET_IM_CONFIGURATION		0x73

#define	INSTEON_HIGHEST_COMMAND				0x73

//	This array contains repsonse lengths excluding the first two bytes.
//	The array starts at the command 0x50. -1 means variable length response.
//  Variable length responses are terminated by 0x06 or 0x15

static const int responseLength[] = {		9,					// 0x50 standard message
											23,					// 0x51 extended message
											-1,					// 0x52 X10
											8,					// 0x53 ALL linking completed
											1,					// 0x54 button event
											0,					// 0x55 user reset
											5,					// 0x56 ALL link cleanup failure
											8,					// 0x57 ALL-link record response
											1,					// 0x58 ALL-link clenaup status
											0,					// 0x59 unused
											0,					// 0x5A unused
											0,					// 0x5B unused
											0,					// 0x5C unused
											0,					// 0x5D unused
											0,					// 0x5E unused
											0,					// 0x5F unused
											7,					// 0x60 get IM info
											4,					// 0x61 send ALL-link command
											-1,					// 0x62 send standard or extended message
											-1,					// 0x63 send X10
											3,					// 0x64 start ALL-linking
											1,					// 0x65 cancel ALL-linking
											4,					// 0x66 set host device category
											1,					// 0x67 reset IM
											2,					// 0x68 set ack byte
											1,					// 0x69 get first ALL-link record
											1,					// 0x6A get next ALL-link record
											2,					// 0x6B set IM configuration
											1,					// 0x6C get ALL-link record for sender
											1,					// 0x6D LED on
											1,					// 0x6E LED off
											10,					// 0x6F manage ALL-link record
											2,					// 0x70 set nak byte
											3,					// 0x71 set ack two bytes
											1,					// 0x72 RF sleep
											4					// 0x73 get IM config
									};


//	Device categories

#define INSTEON_DEVICE_CATEGORY_GENERAL_CONTROLLERS			0x00
#define INSTEON_DEVICE_CATEGORY_DIMMABLE_LIGHT_CONTROL		0x01
#define INSTEON_DEVICE_CATEGORY_SWITCHED_LIGHT_CONTROL		0x02
#define INSTEON_DEVICE_CATEGORY_NETWORK_BRIDGES				0x03
#define INSTEON_DEVICE_CATEGORY_CLIMATE_CONTROL				0x05
#define INSTEON_DEVICE_CATEGORY_SENSORS_AND_ACTUATORS		0x07
#define INSTEON_DEVICE_CATEGORY_ENERGY_MANAGEMENT			0x09
#define INSTEON_DEVICE_CATEGORY_WINDOW_COVERINGS			0x0e
#define INSTEON_DEVICE_CATEGORY_ACCESS_CONTROL				0x0f
#define INSTEON_DEVICE_CATEGORY_SENSOR                      0x10

//  Special device subcategories

#define INSTEON_DEVICE_SUBCAT_MOTION_SENSOR                 0x01
#define INSTEON_DEVICE_SUBCAT_OPENCLOSE_SENSOR1             0x02
#define INSTEON_DEVICE_SUBCAT_OPENCLOSE_SENSOR2             0x07
#define INSTEON_DEVICE_SUBCAT_LEAK_SENSOR                   0x08


InsteonDriver::InsteonDriver() : SyntroThread("Insteon", "Insteon")
{
	m_port = NULL;
	m_state = INSTEON_STATE_IDLE;
    m_trace = true;
}

void InsteonDriver::initThread()
{
    QSettings *settings = SyntroUtils::getSettings();
    settings->beginGroup(INSTEON_SETTINGS_PLM);

    if (!settings->contains(INSTEON_SETTINGS_PLM_PORT))
#ifdef Q_OS_WIN
        settings->setValue(INSTEON_SETTINGS_PLM_PORT, "COM3");
#else
        settings->setValue(INSTEON_SETTINGS_PLM_PORT, "/dev/ttyUSB0");
#endif

    if (!settings->contains(INSTEON_SETTINGS_PLM_LATITUDE))
        settings->setValue(INSTEON_SETTINGS_PLM_LATITUDE, 40.77);

    if (!settings->contains(INSTEON_SETTINGS_PLM_LONGITUDE))
        settings->setValue(INSTEON_SETTINGS_PLM_LONGITUDE, 73.96);

    m_portName = settings->value(INSTEON_SETTINGS_PLM_PORT).toString();
    m_latitude = settings->value(INSTEON_SETTINGS_PLM_LATITUDE).toDouble();
    m_longitude = settings->value(INSTEON_SETTINGS_PLM_LONGITUDE).toDouble();

    settings->endGroup();
    settings->sync();    
	delete settings;

    m_state = INSTEON_STATE_WFCONNECT;
    m_lastConnect = SyntroClock() - INSTEON_CONNECT_RETRY;

    m_timer = startTimer(INSTEONDRIVER_INTERVAL);

    srand(SyntroClock());
    calculateSunriseSunset();
    m_lastSyntroUpdate = SyntroClock();
}

void InsteonDriver::finishThread()
{
	QMutexLocker lock(&m_lock);
    killTimer(m_timer);
	disconnect();
}

void InsteonDriver::timerEvent(QTimerEvent * /*event*/)
{
	QMutexLocker lock(&m_lock);
	background();
}

void InsteonDriver::onReadyRead()
{
    QByteArray data;
	int length;
	int partialLength;
	char current;

    length = m_port->bytesAvailable();
    data.resize(length);
    m_port->read(data.data(), data.size());
 
	for (int i = 0; i < length; i++) {
		current = data.at(i);
		partialLength = m_partialResponse.length();
		if (partialLength == 0) {							// looking for 0x02 as start
			if (current != 0x02)
				continue;
			m_partialResponse.append(0x02);
			continue;
		}
		if (partialLength == 1) {							// this should be the command code
			if ((current < INSTEON_LOWEST_COMMAND) || (current > INSTEON_HIGHEST_COMMAND)) {
                qDebug() << QTime::currentTime().toString() << " Illegal Insteon command " << current;
				m_partialResponse.clear();
				continue;
			}
			m_partialResponse.append(current);
			m_partialLeft = responseLength[current - INSTEON_LOWEST_COMMAND];
			if (m_partialLeft == 0) {						// just a two byte message
				m_lock.lock();
                displayMessage(m_partialResponse, "Insteon RX: ");
				m_responseQueue.enqueue(m_partialResponse);	// queue it
				m_lock.unlock();
				m_partialResponse.clear();
				continue;
			}
			continue;
		}
		// must be in the data part of the message

		m_partialResponse.append(current);
		if (m_partialLeft > 0) {
			if (--m_partialLeft <= 0) {
				m_lock.lock();
                displayMessage(m_partialResponse, "Insteon RX: ");
				m_responseQueue.enqueue(m_partialResponse);	// queue it
				m_lock.unlock();
				m_partialResponse.clear();
				continue;
			}
		} else {
            if ((current == INSTEON_ACK) || (current == INSTEON_NAK)) {		// ack or nak
				m_lock.lock();
                displayMessage(m_partialResponse, "Insteon RX: ");
				m_responseQueue.enqueue(m_partialResponse);	// queue it
				m_lock.unlock();
				m_partialResponse.clear();
				continue;
			}
		}
	}
}

bool InsteonDriver::setDeviceLevelByIndex(int deviceIndex, unsigned char level)
{
	QMutexLocker lock(&m_lock);

    if ((deviceIndex < 0) || (deviceIndex >= m_devices.count()))
        return false;

    if (m_state != INSTEON_STATE_ACTIVE)
        return false;

    INSTEON_REQUEST ir;

    ir.requestType = INSTEON_REQUEST_TYPE_DIRECT;
    ir.deviceID = m_devices.at(deviceIndex).deviceID;

    if (level > 0)
        ir.cmd1 = 0x11;
    else
        ir.cmd1 = 0x13;

    ir.cmd2 = level;

    m_requestQueue.enqueue(ir);

    return true;
}

void InsteonDriver::setDeviceLevel(int deviceID, int level)
{
	int deviceIndex = findDeviceIndexInDatabase(deviceID);

	if (deviceIndex < 0)
		return;

	setDeviceLevelByIndex(deviceIndex, level);
}

const char *InsteonDriver::getState()
{
	switch (m_state) {
	case INSTEON_STATE_IDLE:
		return "idle";

	case INSTEON_STATE_WFCONNECT:
		return "waiting for connect";

	case INSTEON_STATE_WFPLMINFO:
		return "waiting for PLM info";

	case INSTEON_STATE_WFDEVICES:
		return "waiting for devices";

	case INSTEON_STATE_ACTIVE:
		return "active";

	default:
		return "unknown";
	}
}

QList<InsteonDevice> InsteonDriver::getDeviceList()
{
	QMutexLocker lock(&m_lock);

	return m_devices;
}

QList<InsteonTimer> InsteonDriver::getTimerList()
{
	QMutexLocker lock(&m_lock);

	return m_timers;
}

QStringList InsteonDriver::getDatabase()
{
    QMutexLocker lock(&m_lock);

    QStringList db;
    QString str;

    str.sprintf("Dev Name                  ID     Ct Sc Lev Fl Gr Tout Last time seen");
    db.append(str);
    str.sprintf("--- ----------------      ------ -- -- --- -- -- ---- --------------");
    db.append(str);
    for (int i=0; i < m_devices.count(); i++) {
        str.sprintf("%02d  %-20.20s  %06x %02x %02x %03x %02x %02x %04x %s", i,
                    qPrintable(m_devices.at(i).name),
                    m_devices.at(i).deviceID,
                    m_devices.at(i).deviceCat,
                    m_devices.at(i).deviceSubcat,
                   m_devices.at(i).currentLevel == -1 ? 0xfff : m_devices.at(i).currentLevel,
                    m_devices.at(i).flags,
                    m_devices.at(i).group,
                    m_devices.at(i).timeouts,
					qPrintable(m_devices.at(i).lastTimeSeen.toString()));
         db.append(str);
    }
    return db;
}

QStringList InsteonDriver::getTimers()
{
    QMutexLocker lock(&m_lock);
    QStringList timers;
    QString str;

    str.sprintf("Idx Name                  Time   Arm Trig   Mod DoW Ran Dlt");
    timers.append(str);
    str.sprintf("--- --------------------  -----  --- -----  --  --  --  ---");
    timers.append(str);

    for (int i=0; i < m_timers.count(); i++) {
        str.sprintf("%02d  %-20.20s  %02d:%02d  %02x  %02d:%02d  %02x  %02d  %02d  %03d", i,
                    qPrintable(m_timers[i].name),
                    m_timers[i].time.hour(),
                    m_timers[i].time.minute(),
                    m_timers[i].armed,
                    m_timers[i].triggerTime.hour(),
                    m_timers[i].triggerTime.minute(),
                    m_timers[i].mode,
                    m_timers[i].daysOfWeek,
                    m_timers[i].randomMode,
                    m_timers[i].deltaTime);

        timers.append(str);

        for (int deviceIndex = 0; deviceIndex < m_timers[i].devices.count(); deviceIndex++) {
            int index = findDeviceIndexInDatabase(m_timers[i].devices[deviceIndex].deviceID);

            if (index < 0) {
                str.sprintf("                          Device: %06x not found", m_timers[i].devices[deviceIndex].deviceID);
			}
            else {
                str.sprintf("                          Device: %06x, level %02d", m_timers[i].devices[deviceIndex].deviceID,
                            m_timers[i].devices[deviceIndex].level);
			}

            timers.append(str);
        }

        timers.append("");
    }

    return timers;
}

bool InsteonDriver::addTimer(char *name)
{
    InsteonTimer timer;

    timer.name = name;
    timer.time.setHMS(0, 0, 0);
    timer.daysOfWeek = 0x7f;
    timer.mode = INSTEON_TIMER_MODE_TOD;
    timer.randomMode = false;
    timer.deltaTime = 0;
    timer.armed = true;

	addTimer(timer);

	return true;
}

void InsteonDriver::addTimer(InsteonTimer timer)
{
	QMutexLocker lock(&m_lock);

	m_timers.append(timer);

    saveTimers();
	loadTimers();
}

void InsteonDriver::updateTimer(InsteonTimer timer)
{
	int timerIndex = findTimerIndex(timer.name);

	if (timerIndex < 0)
		return;

	m_timers[timerIndex] = timer;
	saveTimers();
	loadTimers();
}

bool InsteonDriver::removeTimerByIndex(int timerIndex)
{
	QMutexLocker lock(&m_lock);

    if ((timerIndex < 0) || (timerIndex >= m_timers.count()))
        return false;

	m_timers.removeAt(timerIndex);
	saveTimers();
	loadTimers();
    return true;
}

void InsteonDriver::removeTimer(QString name)
{
	removeTimerByIndex(findTimerIndex(name));
}

bool InsteonDriver::addDeviceToTimerByIndex(int timerIndex, int deviceIndex, int level)
{
    QMutexLocker lock(&m_lock);
    InsteonTimerDevice timerDevice;

    if ((timerIndex < 0) || (timerIndex >= m_timers.count()))
        return false;

    if ((deviceIndex < 0) || (deviceIndex >= m_devices.count()))
        return false;

    timerDevice.deviceID = m_devices.at(deviceIndex).deviceID;
    timerDevice.level = level;
    m_timers[timerIndex].devices.append(timerDevice);
    saveTimers();
    return true;
}

int InsteonDriver::findTimerIndex(QString name)
{
	for (int i = 0; i < m_timers.count(); i++) {
		if (m_timers.at(i).name == name)
			return i;
	}

	return -1;
}

void InsteonDriver::addDevice(char *name, char *id, int flags, int group, int cat, int subCat)
{
	InsteonDevice device;
	unsigned int intId;

    sscanf(id, "%x", &intId);

	device.name = name;
	device.deviceID = intId;
	device.flags = flags;
	device.group = group;
	device.deviceCat = cat;
	device.deviceSubcat = subCat;
 
	addDevice(device);
 }

void InsteonDriver::addDevice(InsteonDevice device)
{
	QMutexLocker lock(&m_lock);
	
	upsertDevice(device);
	
	saveDatabase();
	loadDatabase();
}

bool InsteonDriver::removeDeviceByIndex(int deviceIndex)
{
    if ((deviceIndex < 0) || (deviceIndex >= m_devices.count()))
        return false;
    removeDevice(m_devices.at(deviceIndex).deviceID);
    return true;
}

void InsteonDriver::removeDevice(int deviceID)
{
    QMutexLocker lock(&m_lock);

    // delete timer references in memory
    for (int i = 0; i < m_timers.count(); i++) {
        for (int j = 0; j < m_timers[i].devices.count(); j++) {
            if (m_timers[i].devices[j].deviceID == (unsigned int)deviceID)
                m_timers[i].devices.removeAt(j);
        }
    }
    queueRemoveDevice(deviceID);
}

const QString& InsteonDriver::getDeviceName(int deviceID)
{
    QMutexLocker lock(&m_lock);
    static QString noDevice = "not found";

    int index = findDeviceIndexInDatabase(deviceID);
    if (index == -1)
        return noDevice;
    else
        return m_devices[index].name;
}

//----------------------------------------------------------
//
//	Functions below here assume lock already set

void InsteonDriver::background()
{
	qint64 now = SyntroClock();
	QByteArray message;
    InsteonDevice *adr;
    int deviceIndex;
    QJsonArray updateList;
	
    if ((m_port != NULL) && !m_port->isOpen()) {
		if (m_state > INSTEON_STATE_WFCONNECT) {
            qDebug() << QTime::currentTime().toString() << " PLM port closed";
			emit PLMPortClosed();
			reconnect();
		}
	} 
	
	switch (m_state) {
	case INSTEON_STATE_IDLE:
		break;

	case INSTEON_STATE_WFCONNECT:
        if (m_port == NULL) {
            m_port = new QSerialPort(m_portName);
            connect(m_port, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
        }

		if (!SyntroUtils::syntroTimerExpired(now, m_lastConnect, INSTEON_CONNECT_RETRY))
			break;

		if (m_port->open(QIODevice::ReadWrite)) {
			m_port->setBaudRate(QSerialPort::Baud19200);
			m_port->setDataBits(QSerialPort::Data8);
			m_port->setParity(QSerialPort::NoParity);
			m_port->setStopBits(QSerialPort::OneStop);
			m_port->setFlowControl(QSerialPort::NoFlowControl);
            qDebug() << QTime::currentTime().toString() << " PLM port open";
			emit PLMPortOpen();
			sendSimpleCommand(INSTEON_GET_IM_INFO);
			m_state = INSTEON_STATE_WFPLMINFO;
		}
		break;

	case INSTEON_STATE_WFPLMINFO:
		message.clear();

		while (!m_responseQueue.empty()) {
			message = m_responseQueue.dequeue();

			if (message.at(1) == INSTEON_GET_IM_INFO)
				break;

			message.clear();
		}

		if (message.length() == 0) {						// no valid response yet
			if (!SyntroUtils::syntroTimerExpired(now, m_lastRequestTime, INSTEON_REQUEST_TIMEOUT))
				break;

			// timed out request - declare link down

			reconnect();
			break;
		}

		// got correct response type

		decodePLMInfo(message);
		sendSimpleCommand(INSTEON_GET_FIRST_ALL_LINK_RECORD);
		m_state = INSTEON_STATE_WFDEVICES;
		break;

	case INSTEON_STATE_WFDEVICES:				
		message.clear();

		while (!m_responseQueue.empty()) {
			message = m_responseQueue.dequeue();

			if (message.at(1) == INSTEON_ALL_LINK_RECORD_RESPONSE)
				break;

			if ((message.at(1) == INSTEON_GET_FIRST_ALL_LINK_RECORD) ||
				(message.at(1) == INSTEON_GET_NEXT_ALL_LINK_RECORD)) {

				// this is the direct response to the request
                if (message.at(2) == INSTEON_NAK) {
					// there are no more devices
					m_state = INSTEON_STATE_ACTIVE;
                    m_requestOutstanding = false;
                    m_expectingStandardMessage = false;
                    m_lastRequestTime = now;
                    m_lastDevicePollTime = now;
                    m_deviceToPoll = 0;
                    m_lastTimerCheckMinute = -1;
                    loadDatabase();
                    saveDatabase();
                    loadTimers();
                    qDebug() << QTime::currentTime().toString() << "Timers active";
					break;
				}
			}

			message.clear();
		}

		if (m_state == INSTEON_STATE_ACTIVE)
			break;

		if (message.length() == 0) {						// no valid response yet
			if (!SyntroUtils::syntroTimerExpired(now, m_lastRequestTime, INSTEON_REQUEST_TIMEOUT))
				break;

			// timed out request - declare link down
			reconnect();
			break;
		}

		// got correct response type

        if ((adr = decodeALLLinkRecord(message)) == NULL) {
			// decode failed - try again
            qDebug() << QTime::currentTime().toString() << " Insteon: incorrect ALL link record";
			reconnect();
            break;
        }

		if ((deviceIndex = findDeviceIndexInDatabase(adr->deviceID)) == -1) {
			m_devices.append(*adr);							// it wasn't there before
		} else {
			m_devices[deviceIndex].deviceCat = adr->deviceCat;
			m_devices[deviceIndex].deviceSubcat = adr->deviceSubcat;
		}

		delete adr;

 //       qDebug() << QTime::currentTime().toString() << " Insteon: added" << QString::number(adr->deviceID, 16);
		sendSimpleCommand(INSTEON_GET_NEXT_ALL_LINK_RECORD);
		break;

	case INSTEON_STATE_ACTIVE:
		processResponses();

        if (m_requestOutstanding) {
            if (SyntroUtils::syntroTimerExpired(now, m_lastRequestTime, INSTEON_REQUEST_TIMEOUT)) {
                qDebug() << QTime::currentTime().toString() << " Insteon: timed out request to PLM";
                reconnect();
                break;
			}
        } else {
            if (m_expectingStandardMessage) {
                if (SyntroUtils::syntroTimerExpired(now, m_lastRequestTime, INSTEON_REQUEST_TIMEOUT)) {
                    if ((deviceIndex = findDeviceIndexInDatabase(m_messageDeviceID)) != -1)
                        m_devices[deviceIndex].timeouts++;
                    m_expectingStandardMessage = false;
                    break;
                }
            } else {
                if (!m_requestQueue.empty())
                    sendQueuedRequest();
            }
        }
        if (!m_requestOutstanding && !m_expectingStandardMessage && (m_devices.count() > 0) &&
                SyntroUtils::syntroTimerExpired(now, m_lastDevicePollTime, INSTEON_POLL_INTERVAL)) {

			int loopCount = 0;

			while (loopCount < m_devices.count()) {
				if (m_deviceToPoll >= m_devices.count())
					m_deviceToPoll = 0;

				if (m_devices[m_deviceToPoll].poll)
					break;

				m_deviceToPoll++;

				loopCount++;
			}

			if (loopCount < m_devices.count()) {
				// this device is ready to poll
				queueLevelRequest(m_devices.at(m_deviceToPoll).deviceID);
				m_lastDevicePollTime = now;

				if (++m_deviceToPoll >= m_devices.count())
					 m_deviceToPoll = 0;
			}
        }
    
	    processTimers();
        if (SyntroUtils::syntroTimerExpired(now, m_lastSyntroUpdate, INSTEON_SYNTRO_UPDATE_INTERVAL)) {
            m_lastSyntroUpdate = now;
            for (int i = 0; i < m_devices.count(); i++) {
                QJsonObject jso;
                m_devices[i].write(jso);
                updateList.append(jso);
            }
            QJsonObject jsonUpdate;
            jsonUpdate[INSTEON_DEVICE_JSON_UPDATELIST] = updateList;
            emit newSyntroUpdate(jsonUpdate);
        }
    }
}

void InsteonDriver::processTimers()
{
	INSTEON_REQUEST ir;

    QTime currentTime = QTime::currentTime();
 
    // Check to see if too soon

    if (currentTime.minute() == m_lastTimerCheckMinute)
        return;                                             // alrwady done this minute

    m_lastTimerCheckMinute = currentTime.minute();          // stops check again until next minute

    // Check to see if midnight - if so, reset fired signals.

    if ((currentTime.hour() == 0) && (currentTime.minute() == 0)) {
        // it is midnight

        calculateSunriseSunset();                           // recalculate for new day

        unsigned char armMask = 1 << (QDate::currentDate().dayOfWeek() - 1); // Monday is bit 0, Sunday is bit 6

        for (int i = 0; i < m_timers.count(); i++) {
            if (armMask & m_timers.at(i).daysOfWeek) {
                m_timers[i].armed = true;                  // arm if its a day to arm
                m_timers[i].triggerTime = getTriggerTime(m_timers[i]);                // set its trigger time
            }
        }
    }

    // Now see if any armed timers time has come

    for (int timerIndex = 0; timerIndex < m_timers.count(); timerIndex++) {
        if (!m_timers[timerIndex].armed)
            continue;

        if (m_timers[timerIndex].triggerTime > currentTime)
            continue;                                       // not time yet

        m_timers[timerIndex].armed = false;

        for (int deviceIndex = 0; deviceIndex < m_timers[timerIndex].devices.count(); deviceIndex++) {

            int level = m_timers[timerIndex].devices[deviceIndex].level;

            ir.requestType = INSTEON_REQUEST_TYPE_DIRECT;
            ir.deviceID = m_timers[timerIndex].devices[deviceIndex].deviceID;

            if (level > 0)
                ir.cmd1 = 0x11;
            else
                ir.cmd1 = 0x13;

            ir.cmd2 = level;

            qDebug() << QTime::currentTime().toString() << " setting device " << QString::number(ir.deviceID, 16) << "to level " << level;
            m_requestQueue.enqueue(ir);
        }
    }
}

void InsteonDriver::sendQueuedRequest()
{
	if (m_requestQueue.isEmpty())
		return;

	INSTEON_REQUEST ir = m_requestQueue.dequeue();

    m_expectingStandardMessage = false;

    switch (ir.requestType) {
    case INSTEON_REQUEST_TYPE_DIRECT:
        if ((ir.cmd1 == 0x11) || (ir.cmd1 == 0x13) || (ir.cmd1 == 0x19)) {
            m_expectingStandardMessage = true;
            m_messageDeviceID = ir.deviceID;
        }
        sendDirectMessage(ir.deviceID, ir.cmd1, ir.cmd2);
        break;

    case INSTEON_REQUEST_TYPE_ADD_DEVICE:
        sendAddDeviceMessage(ir.deviceID, true);
        break;

    case INSTEON_REQUEST_TYPE_REMOVE_DEVICE:
        sendAddDeviceMessage(ir.deviceID, false);
        break;

    default:
        qDebug() << QTime::currentTime().toString() << " Unexpected request type " << ir.requestType;
        break;
    } 
}

void InsteonDriver::processResponses()
{
	QByteArray message;

	while (!m_responseQueue.empty()) {
		message = m_responseQueue.dequeue();

		switch (message.at(1)) {
		case INSTEON_GET_IM_INFO:
            m_requestOutstanding = false;
            decodePLMInfo(message);
            break;

        case INSTEON_SEND_MESSAGE:
            m_requestOutstanding = false;
            break;

        case INSTEON_STANDARD_MESSAGE_RECEIVED:
            processStandardMessage(message);
            break;

		case INSTEON_ALL_LINKING_COMPLETED:
			processALLLinkingCompleted(message);
			break;

        case INSTEON_MANAGE_ALL_LINK_RECORD:
            m_requestOutstanding = false;
            if (message.data()[10] == INSTEON_NAK)
                qDebug() << QTime::currentTime().toString() << " PLM rejected add/remove device for " << getDeviceID((const unsigned char *)message.constData() + 5);
            break;

		default:
			break;
		}
	}
}

void InsteonDriver::processStandardMessage(const QByteArray &message)
{
    int index = findDeviceIndexInDatabase((const unsigned char *)message.data() + 2);

    if (index < 0) {
		displayMessage(message, " Message from device not in the database: ");
        return;                                             // wasn't found
	}

	m_devices[index].lastTimeSeen = QDateTime::currentDateTime();

	if (isBroadcastMessage(message)) {
        if (m_devices[index].deviceCat == INSTEON_DEVICE_CATEGORY_SENSOR) {
			switch (m_devices[index].deviceSubcat) {
            case INSTEON_DEVICE_SUBCAT_MOTION_SENSOR:
                if ((message.at(9) == 0x11) && (m_devices[index].currentLevel != 0xff)) {
                    m_devices[index].currentLevel = 0xff;
                    qDebug() << QTime::currentTime().toString() << " Motion detected by " << m_devices[index].name;
                }
                if ((message.at(9) == 0x13) && (m_devices[index].currentLevel != 0)) {
                    m_devices[index].currentLevel = 0;
                    qDebug() << QTime::currentTime().toString() << " Motion no longer detected by " << m_devices[index].name;
                }
                break;

            case INSTEON_DEVICE_SUBCAT_OPENCLOSE_SENSOR1:
            case INSTEON_DEVICE_SUBCAT_OPENCLOSE_SENSOR2:
                if ((message.at(9) == 0x11) && (m_devices[index].currentLevel != 0xff)) {
                    m_devices[index].currentLevel = 0xff;
                    qDebug() << QTime::currentTime().toString() << " Door open detected by " << m_devices[index].name;
                }

                if ((message.at(9) == 0x13) && (m_devices[index].currentLevel != 0)) {
                    m_devices[index].currentLevel = 0;
                    qDebug() << QTime::currentTime().toString() << " Door close detected by " << m_devices[index].name;
                }
                break;

            case INSTEON_DEVICE_SUBCAT_LEAK_SENSOR:
                if ((message.at(9) == 0x11) && (message.at(10) == 2)) {
                    m_devices[index].currentLevel = 0xff;
                    qDebug() << QTime::currentTime().toString() << " Leak detected by " << m_devices[index].name;
                }

                if (message.at(9) == 0x13) {
                    m_devices[index].currentLevel = 0xff;
                    qDebug() << QTime::currentTime().toString() << " Leak detected by " << m_devices[index].name;
                }
                break;
            }
		}

		return;
	}

   if (m_expectingStandardMessage) {
		index = findDeviceIndexInDatabase(m_messageDeviceID);
		
		if (index >= 0) {
			m_expectingStandardMessage = false;
			m_devices[index].currentLevel = (unsigned char)message.at(10);
		}
	}
}

void InsteonDriver::processALLLinkingCompleted(const QByteArray &message)
{
    InsteonDevice device;

	int index = findDeviceIndexInDatabase((const unsigned char *)message.data() + 4);

    if (index >= 0) {
		displayMessage(message, " ALL Linking completed but device in table: ");
        return;                                             // was found
	}

    device.flags = 0xe2;
    device.group = message.at(3);
    device.deviceID = getDeviceID((const unsigned char *)(message.constData() + 4));
    device.deviceCat = message.at(7);
    device.deviceSubcat = message.at(8);
    device.currentLevel = -1;
	device.timeouts = 0;
	device.lastTimeSeen = QDateTime::currentDateTime();
	device.poll = getPollState(device.deviceCat);
	m_devices.append(device);
	saveDatabase();
}

void InsteonDriver::newPortName(QString portName)
{
    QSettings *settings = SyntroUtils::getSettings();
    settings->beginGroup(INSTEON_SETTINGS_PLM);
    settings->setValue(INSTEON_SETTINGS_PLM_PORT, portName);
	settings->endGroup();
	delete settings;
	m_portName = portName;
	reconnect();
}

void InsteonDriver::reconnect()
{
    qDebug() << QTime::currentTime().toString() << " Insteon: reconnect";
    m_timers.clear();
	m_port->close();
	delete m_port;
	m_port = NULL;
	m_partialResponse.clear();
	m_devices.clear();
	m_requestQueue.clear();
    m_responseQueue.clear();
    m_state = INSTEON_STATE_WFCONNECT;
}

void InsteonDriver::disconnect()
{
    qDebug() << QTime::currentTime().toString() << " Insteon: disconnect";

	if (m_port == NULL)
		return;

    m_timers.clear();
	m_port->close();
	delete m_port;
	m_port = NULL;
	m_partialResponse.clear();
	m_devices.clear();
	m_requestQueue.clear();
    m_responseQueue.clear();
    m_state = INSTEON_STATE_IDLE;
}

bool InsteonDriver::decodePLMInfo(const QByteArray& message)
{
	if (message.length() != responseLength[INSTEON_GET_IM_INFO - INSTEON_LOWEST_COMMAND])
		return false;

	m_plm.deviceID = getDeviceID((const unsigned char *)(message.constData() + 2));
	m_plm.category = message.at(5);
	m_plm.subCategory = message.at(6);
	m_plm.firmwareRev = message.at(7);
	return true;
}

InsteonDevice *InsteonDriver::decodeALLLinkRecord(const QByteArray& message)
{
	if (message.length() != 10)
		return NULL;

    InsteonDevice *adr = new InsteonDevice;

    adr->flags = message.at(2);
    adr->group = message.at(3);
    adr->deviceID = getDeviceID((const unsigned char *)(message.constData() + 4));
    adr->deviceCat = message.at(7);
    adr->deviceSubcat = message.at(8);
    adr->currentLevel = -1;
	adr->timeouts = 0;
	adr->lastTimeSeen = QDateTime::currentDateTime();
	adr->poll = getPollState(adr->deviceCat);
    return adr;
}

unsigned int InsteonDriver::getDeviceID(const unsigned char *id)
{
    return (((unsigned int)id[0]) << 16) + (((unsigned int)id[1]) << 8) + (unsigned int)id[2];
}

void InsteonDriver::setDeviceID(unsigned int intId, unsigned char *id)
{
	id[0] = intId >> 16;
	id[1] = intId >> 8;
	id[2] = intId;
}

int InsteonDriver::findDeviceIndexInDatabase(const unsigned char *id)
{
	unsigned int intId = getDeviceID(id);

	return findDeviceIndexInDatabase(intId);
}

int InsteonDriver::findDeviceIndexInDatabase(unsigned int intId)
{
    for (int i = 0; i < m_devices.count(); i++) {
        if (m_devices.at(i).deviceID == intId)
            return i;
    }

    return -1;
}

bool InsteonDriver::getPollState(int deviceCat)
{
	switch (deviceCat) {
	case INSTEON_DEVICE_CATEGORY_SENSOR:
	case INSTEON_DEVICE_CATEGORY_CLIMATE_CONTROL:
	case INSTEON_DEVICE_CATEGORY_WINDOW_COVERINGS:
		return false;
	}

	return true;
}

void InsteonDriver::queueLevelRequest(unsigned int deviceID)
{
    INSTEON_REQUEST ir;
    ir.requestType = INSTEON_REQUEST_TYPE_DIRECT;
    ir.deviceID = deviceID;
    ir.cmd1 = 0x19;
    ir.cmd2 = 0;
    m_requestQueue.enqueue(ir);
}

void InsteonDriver::queueAddDevice(unsigned int deviceID)
{
    INSTEON_REQUEST ir;
    ir.requestType = INSTEON_REQUEST_TYPE_ADD_DEVICE;
    ir.deviceID = deviceID;
    ir.cmd1 = 0;
    ir.cmd2 = 0;
    m_requestQueue.enqueue(ir);
}

void InsteonDriver::queueRemoveDevice(unsigned int deviceID)
{
    INSTEON_REQUEST ir;
    ir.requestType = INSTEON_REQUEST_TYPE_REMOVE_DEVICE;
    ir.deviceID = deviceID;
    ir.cmd1 = 0;
    ir.cmd2 = 0;
    m_requestQueue.enqueue(ir);
}

bool InsteonDriver::sendSimpleCommand(char command)
{
	char data[2];

	if (!m_port->isOpen())
		return false;

    if (m_trace)
        qDebug() << QTime::currentTime().toString() << " Insteon: sending " << QString::number((unsigned char)command, 16);

	m_lastRequestTime = SyntroClock();
	m_requestOutstanding = true;

	data[0] = 2;
	data[1] = command;
    displayMessage(QByteArray(data, 2), "Insteon TX: ");

	return (m_port->write(data, 2) == 2);
}

bool InsteonDriver::sendDirectMessage(unsigned int deviceID, unsigned char cmd1, unsigned char cmd2)
{
    unsigned char data[8];

    if (!m_port->isOpen())
        return false;

    if (m_state != INSTEON_STATE_ACTIVE)
        return false;

    int deviceIndex = findDeviceIndexInDatabase(deviceID);

    if (deviceIndex == -1)
        return false;

    m_lastRequestTime = SyntroClock();
	m_requestOutstanding = true;

    data[0] = 2;
    data[1] = INSTEON_SEND_MESSAGE;
    setDeviceID(deviceID, data + 2);
    data[5] = 0x0f;
    data[6] = cmd1;
    data[7] = cmd2;

    displayMessage(QByteArray((const char *)data, 8), "Insteon TX: ");

    return (m_port->write((char *)data, 8) == 8);
}

bool InsteonDriver::sendAddDeviceMessage(unsigned int deviceID, bool add)
{
    unsigned char data[11];

    if (!m_port->isOpen())
        return false;

    if (m_state != INSTEON_STATE_ACTIVE)
        return false;

    int deviceIndex = findDeviceIndexInDatabase(deviceID);

    if (deviceIndex == -1)
        return false;

    m_lastRequestTime = SyntroClock();
	m_requestOutstanding = true;

    if (add)
        qDebug() << QTime::currentTime().toString() << " PLM adding " << QString::number(m_devices.at(deviceIndex).deviceID, 16);
    else
        qDebug() << QTime::currentTime().toString() << " PLM removing " << QString::number(m_devices.at(deviceIndex).deviceID, 16);

    data[0] = 2;
    data[1] = INSTEON_MANAGE_ALL_LINK_RECORD;

    if (add)
        data[2] = 0x41;
    else
        data[2] = 0x80;

    data[3] = m_devices.at(deviceIndex).flags;
    data[4] = m_devices.at(deviceIndex).group;
    setDeviceID(m_devices.at(deviceIndex).deviceID, data + 5);
    data[8] = m_devices.at(deviceIndex).deviceCat;
    data[9] = m_devices.at(deviceIndex).deviceSubcat;
    data[10] = 0;

    displayMessage(QByteArray((const char *)data, 11), "Insteon TX: ");

    // delete device from database if deleting a device

    if (!add) {
		m_devices.removeAt(deviceIndex);
        QSettings *settings = SyntroUtils::getSettings();
        settings->beginGroup(INSTEON_SETTINGS_DEVICE_DATABASE);
        settings->remove(QString::number(m_devices.at(deviceIndex).deviceID, 16));
        settings->endGroup();
        settings->sync();
        delete settings;
    }

    return (m_port->write((char *)data, 11) == 11);
}

void InsteonDriver::displayMessage(const QByteArray& message, const char *prefix)
{
    if (!m_trace)
        return;

    QString str = prefix;
	QString strTmp;

	for (int i = 0; i < message.length(); i++)
		str += strTmp.sprintf("%02X", (unsigned char)message.at(i));

    qDebug() << str;
}



void InsteonDriver::upsertDevice(InsteonDevice rec)
{
    rec.timeouts = 0;
    rec.currentLevel = -1;
    rec.poll = getPollState(rec.deviceCat);

	int deviceIndex = findDeviceIndexInDatabase(rec.deviceID);

    if (deviceIndex < 0) {
        // entry in database but not in PLM - create entry and send to PLM
        m_devices.append(rec);
        queueAddDevice(m_devices.count() - 1);
    } else {
        // entry in both database and PLM.
        if (!deviceChanged(&rec, &m_devices[deviceIndex])) {
            m_devices[deviceIndex].name = rec.name;
            m_devices[deviceIndex].group = rec.group;
            m_devices[deviceIndex].flags = rec.flags;
        } else {
            // new database device is different - force this instead
            m_devices[deviceIndex] = rec;
            queueAddDevice(deviceIndex);
        }
    }
}

bool InsteonDriver::deviceChanged(InsteonDevice *a, InsteonDevice *b)
{
    if (a->deviceCat != b->deviceCat)
        return true;

    if (a->deviceSubcat != b->deviceSubcat)
        return true;

    return false;
}

void InsteonDriver::loadDatabase()
{
    bool ok;

    InsteonDevice id;
    int intId;
    int index;

    QSettings *settings = SyntroUtils::getSettings();
    settings->beginGroup(INSTEON_SETTINGS_DEVICE_DATABASE);
    QStringList devices = settings->childGroups();

    for (int i = 0; i < devices.count(); i++) {
        settings->beginGroup(devices.at(i));
        intId = settings->value(INSTEON_SETTINGS_DEVICE_ID).toString().toInt(&ok, 16);
        if ((index = findDeviceIndexInDatabase(intId)) == -1) {
            // entry in .ini but not in PLM - create entry and send to PLM
            id.deviceID = intId;
            id.name = settings->value(INSTEON_SETTINGS_DEVICE_NAME).toString();
            id.flags = settings->value(INSTEON_SETTINGS_DEVICE_FLAGS).toInt();
            id.group = settings->value(INSTEON_SETTINGS_DEVICE_GROUP).toInt();
            id.deviceCat = settings->value(INSTEON_SETTINGS_DEVICE_CAT).toInt();
            id.deviceSubcat = settings->value(INSTEON_SETTINGS_DEVICE_SUBCAT).toInt();
            id.poll = getPollState(id.deviceCat);
            id.timeouts = 0;
            m_devices.append(id);
            queueAddDevice(m_devices.count() - 1);
        } else {
            // entry in both .ini and PLM. Just copy name into database from .ini
            m_devices[index].name = settings->value(INSTEON_SETTINGS_DEVICE_NAME).toString();
        }
        settings->endGroup();
    }
    settings->endGroup();
    qDebug() << "Devices: " << devices.count() << "m_devices: " << m_devices.count();
}

void InsteonDriver::saveDatabase()
{
	InsteonDevice id;

	QSettings *settings = SyntroUtils::getSettings();
	settings->beginGroup(INSTEON_SETTINGS_DEVICE_DATABASE);

	for (int i = 0; i < m_devices.count(); i++) {
		id = m_devices.at(i);

		settings->beginGroup(QString::number(id.deviceID, 16));

        settings->setValue(INSTEON_SETTINGS_DEVICE_ID, QString::number(id.deviceID, 16));
		settings->setValue(INSTEON_SETTINGS_DEVICE_NAME, id.name);
		settings->setValue(INSTEON_SETTINGS_DEVICE_FLAGS, id.flags);
		settings->setValue(INSTEON_SETTINGS_DEVICE_GROUP, id.group);
		settings->setValue(INSTEON_SETTINGS_DEVICE_CAT, id.deviceCat);
		settings->setValue(INSTEON_SETTINGS_DEVICE_SUBCAT, id.deviceSubcat);

		settings->endGroup();
	}

	settings->endGroup();
	settings->sync();
	delete settings;
}

void InsteonDriver::loadTimers()
{	
    bool ok;

	QTime currentTime = QTime::currentTime();
	m_timers.clear();


	QSettings *settings = SyntroUtils::getSettings();
	int timerCount = settings->beginReadArray(INSTEON_SETTINGS_TIMERS);
	for (int timerIndex = 0; timerIndex < timerCount; timerIndex++) {
		InsteonTimer timer;
        InsteonTimerDevice timerDevice;

        settings->setArrayIndex(timerIndex);
		timer.name = settings->value(INSTEON_SETTINGS_TIMERS_NAME).toString();
		timer.time = QTime(settings->value(INSTEON_SETTINGS_TIMERS_TIMEHOUR).toInt(),
               settings->value(INSTEON_SETTINGS_TIMERS_TIMEMINUTE).toInt());
		timer.daysOfWeek = settings->value(INSTEON_SETTINGS_TIMERS_DOW).toInt();
        timer.randomMode = settings->value(INSTEON_SETTINGS_TIMERS_RANDOM).toBool();
		timer.mode = settings->value(INSTEON_SETTINGS_TIMERS_MODE).toInt();
		timer.deltaTime = settings->value(INSTEON_SETTINGS_TIMERS_DELTATIME).toInt();
		timer.triggerTime = getTriggerTime(timer);
		timer.armed = timer.triggerTime >= currentTime;

		int deviceCount = settings->beginReadArray(INSTEON_SETTINGS_TIMERS_DEVICES);
		for (int deviceIndex = 0; deviceIndex < deviceCount; deviceIndex++) {
			settings->setArrayIndex(deviceIndex);
            timerDevice.deviceID = settings->value(INSTEON_SETTINGS_TIMERS_DEVICES_ID).toString().toInt(&ok, 16);
			timerDevice.level = settings->value(INSTEON_SETTINGS_TIMERS_DEVICES_LEVEL).toInt();
			timer.devices.append(timerDevice);
		}
		m_timers.append(timer);
		settings->endArray();
	}
	settings->endArray();
	delete settings;
}

void InsteonDriver::saveTimers()
{
	QSettings *settings = SyntroUtils::getSettings();
	settings->beginWriteArray(INSTEON_SETTINGS_TIMERS);
	for (int timerIndex = 0; timerIndex < m_timers.count(); timerIndex++) {
		settings->setArrayIndex(timerIndex);

        InsteonTimer timer;
        InsteonTimerDevice timerDevice;

        timer = m_timers.at(timerIndex);
		settings->setValue(INSTEON_SETTINGS_TIMERS_NAME, timer.name);
		settings->setValue(INSTEON_SETTINGS_TIMERS_TIMEHOUR, timer.time.hour());
		settings->setValue(INSTEON_SETTINGS_TIMERS_TIMEMINUTE, timer.time.minute());
		settings->setValue(INSTEON_SETTINGS_TIMERS_DOW, timer.daysOfWeek);
		settings->setValue(INSTEON_SETTINGS_TIMERS_RANDOM, timer.randomMode);
		settings->setValue(INSTEON_SETTINGS_TIMERS_MODE, timer.mode);
		settings->setValue(INSTEON_SETTINGS_TIMERS_DELTATIME, timer.deltaTime);

		settings->beginWriteArray(INSTEON_SETTINGS_TIMERS_DEVICES);
        settings->remove("");
		for (int deviceIndex = 0; deviceIndex < timer.devices.count(); deviceIndex++) {
			settings->setArrayIndex(deviceIndex);
			timerDevice = timer.devices.at(deviceIndex);
            settings->setValue(INSTEON_SETTINGS_TIMERS_DEVICES_ID, QString::number(timerDevice.deviceID, 16));
			settings->setValue(INSTEON_SETTINGS_TIMERS_DEVICES_LEVEL, timerDevice.level);
		}
		settings->endArray();
	}

	settings->endArray();
	settings->sync();
	delete settings;
}

void InsteonDriver::calculateSunriseSunset()
{
//    QTime timeUTC = QDateTime::currentDateTimeUtc().time();
//    double jd = jdn + (double)(timeUTC.hour() - 12) / 24.0 + (double)(timeUTC.minute()) / 1440;

    QDate dateUTC = QDateTime::currentDateTimeUtc().date();
    double pi = 3.14159265;

    double jdn = (double(dateUTC.toJulianDay()));


    double jStar = m_longitude / 360.0 + jdn;

    double solarMeanAnomaly = fmod((357.5291 + 0.98560028 * (jStar - 2451545)), 360.0);

    double smr = (solarMeanAnomaly / 180.0) * pi;

    double equationOfCenter = 1.9148 * sin(smr) + 0.02 * sin(2 * smr) + 0.0003 * sin(3.0 * smr);

    double eclipticLongitude = fmod((solarMeanAnomaly + 102.9372 + equationOfCenter + 180), 360);

    double jTransit = jStar + 0.0053 * sin(smr) - 0.0069 * sin(2.0 * eclipticLongitude);

    double sunDec = sin((eclipticLongitude / 180.0) * pi) * sin((23.45 / 180.0) * pi);

    double hourAngle = (180.0 / pi) * acos((sin((-0.83 / 180.0) * pi) - sin((m_latitude / 180.0) * pi) * sunDec) /
            (cos((m_latitude/180.0) * pi) * cos(asin(sunDec))));

    double jSet = jdn + (hourAngle + m_longitude) / 360.0 + 0.0053 * sin(smr) - 0.0069 * sin(2.0 * (eclipticLongitude / 180.0) * pi);

    double jRise = jTransit - (jSet - jTransit);

    m_sunrise = convertJDToQTime(jRise);
    m_sunset = convertJDToQTime(jSet);
    qDebug() << "Sunrise: " << m_sunrise.toString();
    qDebug() << "Sunset: " << m_sunset.toString();
 }

QTime InsteonDriver::convertJDToQTime(double JD)
{
    QTime timeUTC = QDateTime::currentDateTimeUtc().time();
    QTime timeLocal = QTime::currentTime();
    int timeOffset = timeLocal.hour() - timeUTC.hour();

    qint64 jdn = (qint64) JD;

    JD -= jdn;

    int hour = (int)(JD * 24.0);

    JD -= (double)hour / 24.0;

    int minute = (int)(JD * 24.0 * 60.0);

    hour += 12 + timeOffset;
    if (hour > 24)
        hour -= 24;

    if (hour < 0)
        hour += 24;
    return QTime(hour, minute);
}

void InsteonDriver::displayJD(double JD)
{
    qint64 jdn = (qint64) JD;

    JD -= jdn;

    int hour = (int)(JD * 24.0);

    JD -= (double)hour / 24.0;

    int minute = (int)(JD * 24.0 * 60.0);

    hour += 12;
    if (hour > 24) {
        hour -= 24;
        jdn += 1;
    }

    qDebug() << "Julian Date " << QDate::fromJulianDay(jdn).toString() << " " << hour << ":" << minute;
}

QTime InsteonDriver::getTriggerTime(InsteonTimer timer)
{
    int delta = 0;

    if (timer.randomMode)
        delta = (int)((double)timer.deltaTime * (double)(2 * (rand() - RAND_MAX / 2)) / (double)RAND_MAX );

    switch (timer.mode) {
    case INSTEON_TIMER_MODE_SUNSET:
        return m_sunset.addSecs(delta * 60);
        break;

    case INSTEON_TIMER_MODE_SUNRISE:
        return m_sunrise.addSecs(delta * 60);
	}

	return timer.time.addSecs(delta * 60);
}

const QString& InsteonDriver::getPortName()
{
    return m_portName;
}


