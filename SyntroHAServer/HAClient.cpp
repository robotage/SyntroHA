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

#include <qbytearray.h>
#include <qjsondocument.h>
#include <qbuffer.h>
#include <qjsonarray.h>

#include "HAClient.h"
#include "InsteonDevice.h"

HAClient::HAClient(QObject *)
    : Endpoint(HACLIENT_BACKGROUND_INTERVAL, "HAClient")
{
    m_updatePort = -1;
    m_controlPort = -1;
}

void HAClient::appClientInit()
{
    m_updatePort = clientAddService(SYNTRO_STREAMNAME_HOMEAUTOMATION, SERVICETYPE_MULTICAST, true);
    m_controlPort = clientAddService(SYNTRO_STREAMNAME_CONTROL, SERVICETYPE_E2E, true);
}

void HAClient::appClientExit()
{
    if (m_updatePort != -1)
        clientRemoveService(m_updatePort);
    m_updatePort = -1;
    if (m_controlPort != -1)
        clientRemoveService(m_controlPort);
    m_controlPort = -1;
 }

void HAClient::appClientClosed()
{

}

void HAClient::appClientBackground()
{

}

void HAClient::appClientReceiveE2E(int servicePort, SYNTRO_EHEAD *header, int length)
{
    if (servicePort != m_controlPort) {
        free(header);
        return;
    }

    QByteArray data((char *)(header + 1), length);
    QJsonDocument var(QJsonDocument::fromJson(data));
    QJsonObject newDeviceLevel = var.object();
    free(header);

    if (!newDeviceLevel.contains(INSTEON_DEVICE_JSON_SETDEVICELEVEL))
        return;

    QJsonArray jsa = newDeviceLevel[INSTEON_DEVICE_JSON_SETDEVICELEVEL].toArray();
    for (int i = 0; i < jsa.count(); i++) {
        QJsonObject params = jsa[i].toObject();

        InsteonDevice idev;
        if (!idev.readNewLevel(params))
            continue;
        emit setDeviceLevel(idev.deviceID, idev.newLevel);
    }
}

void HAClient::newSyntroUpdate(QJsonObject updateList)
{
    if (m_updatePort == -1)
        return;
    if (clientIsServiceActive(m_updatePort) && clientClearToSend(m_updatePort)) {

        QByteArray data;
        QBuffer buffer(&data);
        buffer.open(QIODevice::WriteOnly);
        buffer.write(QJsonDocument(updateList).toJson());
        SYNTRO_EHEAD *multiCast = clientBuildMessage(m_updatePort, data.length());
        memcpy((char *)(multiCast + 1), data.data(), data.length());
        clientSendMessage(m_updatePort, multiCast, data.length(), SYNTROLINK_LOWPRI);
    }
}
