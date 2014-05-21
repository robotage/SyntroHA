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

#ifndef _INSTEONDRIVER_H_
#define _INSTEONDRIVER_H_

#include "SyntroLib.h"
#include <qmutex.h>
#include <qqueue.h>
#include <qserialport.h>
#include <qserialportinfo.h>
#include <qdatetime.h>
#include <qjsonobject.h>

#include "InsteonDevice.h"
#include "InsteonTimer.h"
#include "InsteonDefs.h"

#define INSTEONDRIVER_INTERVAL  (SYNTRO_CLOCKS_PER_SEC / 10)

#define INSTEON_CONNECT_RETRY	(SYNTRO_CLOCKS_PER_SEC * 2)

#define INSTEON_REQUEST_TIMEOUT	(SYNTRO_CLOCKS_PER_SEC * 3)

#define INSTEON_POLL_INTERVAL	(SYNTRO_CLOCKS_PER_SEC * 2)

#define INSTEON_TIMER_CHECK_INTERVAL	(SYNTRO_CLOCKS_PER_SEC * 1)

#define INSTEON_SYNTRO_UPDATE_INTERVAL	(SYNTRO_CLOCKS_PER_SEC * 2)

//	state machine states

#define	INSTEON_STATE_IDLE			0					// port closed
#define	INSTEON_STATE_WFCONNECT		1					// waiting to connect to PLM
#define INSTEON_STATE_WFPLMINFO		2					// waiting for PLM info
#define	INSTEON_STATE_WFDEVICES		3					// waiting for device table
#define	INSTEON_STATE_ACTIVE		4					// ready for normal operation


class InsteonDriver : public SyntroThread
{
    Q_OBJECT

public:
    InsteonDriver();
	const char *getState();
    void setTrace(bool on) {m_trace = on;}
    bool getTrace() { return m_trace; }
    QStringList getDatabase();
    QStringList getTimers();
    const QString& getPortName();

    void addDevice(char *name, char *id, int flags, int group, int cat, int subCat);
    bool setDeviceLevelByIndex(int deviceIndex, unsigned char level);
    bool removeDeviceByIndex(int deviceIndex);

    bool addTimer(char *name);
    bool removeTimerByIndex(int timerIndex);
    bool addDeviceToTimerByIndex(int timerIndex, int deviceIndex, int level);
    const QString& getDeviceName(int deviceID);

	QList<InsteonDevice> getDeviceList();
	QList<InsteonTimer> getTimerList();

public slots:
	void addDevice(InsteonDevice device);
	void removeDevice(int deviceID);
	void setDeviceLevel(int deviceID, int level);

	void addTimer(InsteonTimer timer);
	void updateTimer(InsteonTimer timer);
	void removeTimer(QString name);

	void newPortName(QString portName);

signals:
	void PLMPortOpen();
	void PLMPortClosed();
    void newSyntroUpdate(QJsonObject updateList);

protected:
    void initThread();
    void timerEvent(QTimerEvent *event);
    void finishThread();

private slots:
	void onReadyRead();

private:
    void background();
	void reconnect();
	void disconnect();
    void processTimers();
    void calculateSunriseSunset();
    QTime convertJDToQTime(double JD);
    void displayJD(double JD);

    void sendQueuedRequest();
	void processResponses();
    void processStandardMessage(const QByteArray& message);
    void processALLLinkingCompleted(const QByteArray& message);
	bool decodePLMInfo(const QByteArray& message);
    InsteonDevice *decodeALLLinkRecord(const QByteArray& message);
    bool deviceChanged(InsteonDevice *a, InsteonDevice *b);

	unsigned int getDeviceID(const unsigned char *id);
	void setDeviceID(unsigned int intId, unsigned char *id);
	int findDeviceIndexInDatabase(const unsigned char *id);
    int findDeviceIndexInDatabase(unsigned int intId);
	bool getPollState(int deviceCat);
	bool isBroadcastMessage(const QByteArray& message) {return (message.at(5) == 0) && (message.at(6) == 0);}

	int findTimerIndex(QString name);

    void queueLevelRequest(unsigned deviceID);
    void queueAddDevice(unsigned int deviceID);
    void queueRemoveDevice(unsigned int deviceID);

    bool sendSimpleCommand(char command);
    bool sendDirectMessage(unsigned int deviceID, unsigned char cmd1, unsigned char cmd2);
    bool sendAddDeviceMessage(unsigned int deviceID, bool add);

    QTime getTriggerTime(InsteonTimer timer);

    void displayMessage(const QByteArray& message, const char*prefix);

    void loadDatabase();
	void upsertDevice(InsteonDevice rec);
    void saveDatabase();
    void loadTimers();
    void saveTimers();

    QMutex m_lock;
    int m_timer;

	QString m_portName;										// the port name
	QSerialPort *m_port;									// serial port driver
	
	QQueue<QByteArray> m_responseQueue;						// the responses from the PLM

	QByteArray m_partialResponse;							// where a response is built
	int m_partialLeft;										// number of bytes left to complete message

	INSTEON_PLM	m_plm;										// PLM data
    QList<InsteonDevice> m_devices;							// the device list
    QList<InsteonTimer> m_timers;                          // timer records

    QQueue<INSTEON_REQUEST> m_requestQueue;                // queue of outstanding requests

	int m_state;
	qint64 m_lastConnect;									// time of last connect
	qint64 m_lastRequestTime;								// time of last request
    bool m_requestOutstanding;                              // true if request outstanding
    bool m_expectingStandardMessage;                        // if a request should also generate a standard message
    unsigned int m_messageDeviceID;                         // and the device from which it should come

    int m_deviceToPoll;
    qint64 m_lastDevicePollTime;

    int m_lastTimerCheckMinute;

    bool m_trace;

    QTime m_sunset;
    QTime m_sunrise;
    double m_latitude;
    double m_longitude;

    qint64 m_lastSyntroUpdate;                                    // controls sending Syntro updates
};

#endif // _INSTEONDRIVER_H_
