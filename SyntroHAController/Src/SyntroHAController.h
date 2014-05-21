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

#ifndef SYNTROHACONTROLLER_H
#define SYNTROHACONTROLLER_H

#define	PRODUCT_TYPE "SyntroHAController"

#include <QtGui>
#include <qgridlayout.h>
#include <qmainwindow.h>
#include <qstatusbar.h>
#include <qprogressbar.h>
#include <qjsonobject.h>
#include <qtoolbar.h>

#include "SyntroLib.h"
#include "ControllerClient.h"
#include "InsteonDevice.h"

//  settings keys

#define SYNTROHACONTROLLER_SERVER_NAME  "HAServerName"

class MainTable;

class SyntroHAController : public QMainWindow
{
	Q_OBJECT

public:
    SyntroHAController();

public slots:
	void onAbout();
	void onBasicSetup();
    void onExit();
    void onChooseServer();
	void clientConnected();
	void clientClosed();
    void newUpdate(QJsonObject update);
    void cellClicked(int row, int col);
    void serverName(QString serverName);
    void tapAndHold();
	void dirResponse(QStringList directory);

signals:
    void setDeviceLevel(QJsonObject);
    void newSize(QSize);
    void newServerName();
	void requestDir();

protected:
	void closeEvent(QCloseEvent *event);
	void timerEvent(QTimerEvent *event);
    void resizeEvent(QResizeEvent *);

private:
    void layoutWindow();
	void initStatusBar();
	void saveWindowState();
	void restoreWindowState();

    ControllerClient *m_client;

    QLabel *m_controlStatus;
    QLabel *m_serverName;
	QAction *m_chooseServer;

	int m_statusTimer;
	int m_directoryTimer;
	QStringList m_clientDirectory;

	QString m_logTag;

    MainTable *m_table;
    QStatusBar *m_statusBar;
    QToolBar *m_toolBar;

    QMap<int, InsteonDevice> m_map;                         // map from deviceID to device object
    QMap<int, int>m_gridMap;                                // map from deviceID to display row

    bool m_tapSeen;
};

#endif // SYNTROHACONTROLLER_H
