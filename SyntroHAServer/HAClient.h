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

#ifndef HACLIENT_H
#define HACLIENT_H

#include "SyntroLib.h"
#include <qmutex.h>
#include <qjsonobject.h>

#define	HACLIENT_BACKGROUND_INTERVAL (SYNTRO_CLOCKS_PER_SEC/100)

class HAClient : public Endpoint
{
    Q_OBJECT

public:
    HAClient(QObject *parent);

public slots:
    void newSyntroUpdate(QJsonObject updateList);

signals:
    void setDeviceLevel(int, int);

protected:
    void appClientInit();
    void appClientBackground();
    void appClientClosed();
    void appClientExit();
    void appClientReceiveE2E(int servicePort, SYNTRO_EHEAD *header, int length);

private:
    int m_updatePort;
    int m_controlPort;
};

#endif // HACLIENT_H
