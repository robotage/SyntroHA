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

#ifndef CONTROLLERCLIENT_H
#define CONTROLLERCLIENT_H

#include <qjsonobject.h>

#include "SyntroLib.h"

class ControllerClient : public Endpoint
{
	Q_OBJECT

public:
    ControllerClient();

public slots:
    void setDeviceLevel(QJsonObject);
    void newServerName();
	void requestDir();

signals:
	void clientConnected();
	void clientClosed();
    void newUpdate(QJsonObject update);
    void serverName(QString serverName);
	void dirResponse(QStringList directory);

protected:
    void appClientInit();
	void appClientConnected();
	void appClientClosed();
    void appClientReceiveMulticast(int servicePort, SYNTRO_EHEAD *multiCast, int len);
	void appClientReceiveDirectory(QStringList directory);

private:
    int m_updatePort;
    int m_controlPort;
    QString m_serverName;
};

#endif // CONTROLLERCLIENT_H

