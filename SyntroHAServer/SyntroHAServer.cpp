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

#include "SyntroHAServer.h"
#include "HAClient.h"
#include "InsteonDriver.h"

#ifdef WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#include <signal.h>
#endif

#ifndef WIN32
volatile bool SyntroHAServer::sigIntReceived = false;
#endif

SyntroHAServer::SyntroHAServer(bool daemonMode, QObject *parent) : QThread(parent)
{
    m_daemonMode = daemonMode;

#ifndef WIN32
    if (m_daemonMode) {
        registerSigHandler();

        if (daemon(1, 1)) {
            perror("daemon");
            return;
        }
    }
#endif

    SyntroUtils::syntroAppInit();

    m_client = new HAClient(this);
    m_insteonDriver = new InsteonDriver();

    connect(m_insteonDriver, SIGNAL(newSyntroUpdate(QJsonObject)), m_client, SLOT(newSyntroUpdate(QJsonObject)));
    connect(m_client, SIGNAL(setDeviceLevel(int, int)), m_insteonDriver, SLOT(setDeviceLevel(int, int)));

    m_client->resumeThread();
    m_insteonDriver->resumeThread();
	start();
}

void SyntroHAServer::showHelp()
{
	printf("\nOptions are:\n\n");
    printf("  A - add device mode\n");
    printf("  C - control mode\n");
    printf("  D - show device database\n");
    printf("  H - Show help\n");
    printf("  M - Timer management mode\n");
    printf("  R - remove device\n");
    printf("  S - show status\n");
    printf("  T - toggle Insteon trace\n");
	printf("  X - Exit\n");
}

void SyntroHAServer::showStatus()
{
    printf("\nSyntroControl status is: %s\n", qPrintable(m_client->getLinkState()));
    printf("Insteon state: %s\n", m_insteonDriver->getState());
    printf("Insteon trace is %s\n", m_insteonDriver->getTrace() ? "on" : "off");
}

void SyntroHAServer::run()
{
#ifdef WIN32
	runConsole();
#else
    if (m_daemonMode)
        runDaemon();
    else
        runConsole();
#endif

    m_insteonDriver->exitThread();
    m_client->exitThread();

    SyntroUtils::syntroAppExit();
    ((QCoreApplication *)parent())->exit();
}

void SyntroHAServer::addEntryMode()
{
    char str[256], name[256], id[256];
    int cat, subcat, flags, group;

    printf("\nSyntax is <devname> <devid> <flags> <group> <cat> <subcat> or x to exit\n\n");
    while (1) {
        printf("  +: ");
        fgets(str, 256, stdin);
        if (toupper(str[0]) == 'X')
            return;
        if (sscanf(str, "%s %s %d %d %d %d", name, id, &flags, &group, &cat, &subcat) == 6) {
            m_insteonDriver->addDevice(name, id, flags, group, cat, subcat);
            printf("\n* Added %s\n", name);
        }
    }
}

void SyntroHAServer::removeEntry()
{
    char str[256];
    int device;

    printf("\nEntry to remove (enter db index or x to exit): ");
    fgets(str, 256, stdin);
    if (toupper(str[0]) == 'X')
        return;
    if (sscanf(str, "%d", &device) == 1) {
        if (m_insteonDriver->removeDeviceByIndex(device)) {
            printf("\n* Removed %d\n", device);
        } else {
            printf("\n* Failed to remove %d\n", device);
        }
    }
}

void SyntroHAServer::controlMode()
{
    char str[256];
    int device, level;
    int index;

    printf("\nEnter <device> <level> or x to exit. level is between 0 and 255.\n\n");
    while (1) {
        printf("  #: ");
        fgets(str, 256, stdin);
        if (toupper(str[0]) == 'X')
            return;
        index = 0;
        while (index < (int)strlen(str)) {
            if (sscanf(str + index, "%d %d", &device, &level) == 2) {
                m_insteonDriver->setDeviceLevelByIndex(device, level);
                while ((index < (int)strlen(str)) && (str[index] != ' '))
                    index++;
                while ((index < (int)strlen(str)) && (str[index] == ' '))
                    index++;
                while ((index < (int)strlen(str)) && (str[index] != ' '))
                    index++;
                while ((index < (int)strlen(str)) && (str[index] == ' '))
                    index++;
          } else {
                index = strlen(str);
            }

        }
    }
}

void SyntroHAServer::timerManagement()
{
    char str[256], name[256];
    int timerIndex, deviceIndex, level;
    QStringList timers;

    printf("\nCommands availabel are:\n\n");
    printf("  a <timername>                         adds a new timer\n");
    printf("  d <timerindex> <deviceindex> <level>  adds a device to a timer\n");
    printf("  r <timerindex>                        remove a timer entry\n");
    printf("  t                                     prints timer entries\n");
    printf("  x                                     leave timer mode\n");

    while (1) {
        printf("  Timer: ");
        fgets(str, 256, stdin);
        if (strlen(str) < 1)
            continue;

        switch (tolower(str[0])) {
        case 'x' :
            return;

        case 'a':
            if (sscanf(str + 1, "%s", name) == 1) {
                if (m_insteonDriver->addTimer(name))
                    printf("\n* Added %s\n", name);
            }

        case 'd':
            if (sscanf(str + 1, "%d %d %d", &timerIndex, &deviceIndex, &level) == 3) {
                if (m_insteonDriver->addDeviceToTimerByIndex(timerIndex, deviceIndex, level))
                    printf("\n* Added %d to timer %d\n", deviceIndex, timerIndex);
            }
            break;

        case 'r':
            if (sscanf(str + 1, "%d", &timerIndex) == 1) {
                if (m_insteonDriver->removeTimerByIndex(timerIndex))
                    printf("\n* removed timer %d\n", timerIndex);
            }
            break;

        case 't':
            putchar('\n');
            timers = m_insteonDriver->getTimers();
            for (int i = 0; i < timers.count(); i++)
                printf("%s\n", qPrintable(timers.at(i)));
            putchar('\n');
            break;
        }
    }
}


void SyntroHAServer::runConsole()
{
    bool stopRunning = false;
    QStringList db;

#ifndef WIN32
    struct termios	ctty;
	tcgetattr(fileno(stdout), &ctty);
    ctty.c_lflag &= ~(ICANON);
    tcsetattr(fileno(stdout), TCSANOW, &ctty);
#endif
    while (!stopRunning) {
        printf("\nEnter option: ");

#ifdef WIN32
		switch (tolower(_getch()))
#else
        switch (tolower(getchar()))
#endif		
		{
            case 'h':
                showHelp();
                break;

            case 's':
                showStatus();
                break;

            case 'x':
                printf("\nExiting\n");
                stopRunning = true;
                break;

            case 'a':
                addEntryMode();
                break;

            case 'r':
                removeEntry();
                break;

            case 'c':
                controlMode();
                break;

            case 'm':
                timerManagement();
                break;

            case 'd':
                putchar('\n');
                db = m_insteonDriver->getDatabase();
                for (int i = 0; i < db.count(); i++)
                    printf("%s\n", qPrintable(db.at(i)));
                break;

            case 't':
                if (m_insteonDriver->getTrace()) {
                    m_insteonDriver->setTrace(false);
                    printf("\nInsteon trace is off\n");
                } else {
                    m_insteonDriver->setTrace(true);
                    printf("\nInsteon trace is on\n");
                }
            case '\n':
                continue;
        }
    }
}

#ifndef WIN32
void SyntroHAServer::runDaemon()
{
    while (!SyntroHAServer::sigIntReceived)
        msleep(100);
}

void SyntroHAServer::registerSigHandler()
{
    struct sigaction sia;

    bzero(&sia, sizeof sia);
    sia.sa_handler = SyntroHAServer::sigHandler;

    if (sigaction(SIGINT, &sia, NULL) < 0)
        perror("sigaction(SIGINT)");
}

void SyntroHAServer::sigHandler(int)
{
    SyntroHAServer::sigIntReceived = true;
}
#endif
