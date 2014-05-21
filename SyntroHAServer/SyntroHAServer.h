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

#ifndef SYNTROHASERVER_H
#define SYNTROHASERVER_H

#include "SyntroLib.h"
#include <qthread.h>

class HAClient;
class InsteonDriver;

#define PRODUCT_TYPE  "SyntroHAServer"

class SyntroHAServer : public QThread
{
	Q_OBJECT

public:
    SyntroHAServer(bool daemonMode, QObject *parent);

protected:
    void run();
    void addEntryMode();
    void removeEntry();
    void controlMode();
    void timerManagement();
    void showHelp();
    void showStatus();

    void runConsole();
    bool m_daemonMode;

#ifndef WIN32
    void runDaemon();
    void registerSigHandler();
    static void sigHandler(int sig);
    static volatile bool sigIntReceived;
#endif

    HAClient *m_client;
    InsteonDriver *m_insteonDriver;
};

#endif // SYNTROHASERVER_H
