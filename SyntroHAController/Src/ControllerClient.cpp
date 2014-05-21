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

#include "ControllerClient.h"
#include "SyntroHAController.h"

#include <qbuffer.h>
#include <qjsondocument.h>

#define	MOBILECLIENT_BACKGROUND_INTERVAL (SYNTRO_CLOCKS_PER_SEC/10)


ControllerClient::ControllerClient()
    : Endpoint(MOBILECLIENT_BACKGROUND_INTERVAL, "ControllerClient")
{
    m_updatePort = -1;
    m_controlPort = -1;

    QSettings *settings = SyntroUtils::getSettings();

    if (!settings->contains(SYNTROHACONTROLLER_SERVER_NAME))
        settings->setValue(SYNTROHACONTROLLER_SERVER_NAME, "HAServer");
    delete settings;
 }

void ControllerClient::appClientInit()
{
     newServerName();
}

void ControllerClient::appClientConnected()
{
	emit clientConnected();
}

void ControllerClient::appClientClosed()
{
	emit clientClosed();
}

void ControllerClient::appClientReceiveMulticast(int servicePort, SYNTRO_EHEAD *multiCast, int len)
{
    if (servicePort != m_updatePort) {
        free(multiCast);
        return;
    }
    QByteArray data((char *)(multiCast + 1), len);

    QJsonDocument var(QJsonDocument::fromJson(data));

    emit newUpdate(var.object());
	free(multiCast);
}

void ControllerClient::appClientReceiveDirectory(QStringList directory)
{
	emit dirResponse(directory);
}

void ControllerClient::requestDir()
{
	requestDirectory();
}

void ControllerClient::setDeviceLevel(QJsonObject newDeviceLevel)
{
    if (m_controlPort == -1)
        return;

    QByteArray data;
    QBuffer buffer(&data);
    buffer.open(QIODevice::WriteOnly);
    buffer.write(QJsonDocument(newDeviceLevel).toJson());
    SYNTRO_EHEAD *header = clientBuildMessage(m_controlPort, data.length());
    memcpy((char *)(header + 1), data.data(), data.length());
    clientSendMessage(m_controlPort, header, data.length(), SYNTROLINK_LOWPRI);
}

void ControllerClient::newServerName()
{
    if (m_controlPort != -1) {
        clientRemoveService(m_controlPort);
        m_controlPort = -1;
    }
    if (m_updatePort != -1) {
        clientRemoveService(m_updatePort);
        m_updatePort = -1;
    }
    QSettings *settings = SyntroUtils::getSettings();
    QString name = settings->value(SYNTROHACONTROLLER_SERVER_NAME).toString();
    delete settings;

    QString service = SyntroUtils::insertStreamNameInPath(name, SYNTRO_STREAMNAME_HOMEAUTOMATION);
    m_updatePort = clientAddService(service, SERVICETYPE_MULTICAST, false);

    service = SyntroUtils::insertStreamNameInPath(name, SYNTRO_STREAMNAME_CONTROL);
    m_controlPort = clientAddService(service, SERVICETYPE_E2E, false);

    emit serverName(name);
 }
