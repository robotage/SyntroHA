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

#ifndef SYNTROHAWINDOW_H
#define SYNTROHAWINDOW_H

#include <qmainwindow.h>
#include <qtabwidget.h>
#include <qaction.h>

#include "ui_SyntroHAServer.h"

#include "SyntroLib.h"
#include "DeviceView.h"
#include "TimerView.h"

class HAClient;
class InsteonDriver;

class SyntroHAWindow : public QMainWindow
{
	Q_OBJECT

public:
    SyntroHAWindow();

signals:
	void addDevice(InsteonDevice device);
	void removeDevice(int deviceID);
	void setDeviceLevel(int deviceID, int level);

	void addTimer(InsteonTimer timer);
	void updateTimer(InsteonTimer timer);
	void removeTimer(QString name);

	void newPortName(QString portName);

public slots:
	void onBasicSetup();
	void onSerialPortSetup();
	void onAbout();

	void tabChanged(int index);

	void onAddDevice();
	void onEditDevice();
	void onRemoveDevice();

	void onAddTimer();
	void onEditTimer();
	void onRemoveTimer();

	void onAdd();
	void onEdit();
	void onRemove();

	void deviceSelectionChanged();
	void timerSelectionChanged();

	void deviceDoubleClickEvent();

protected:
	void closeEvent(QCloseEvent *);
	void timerEvent(QTimerEvent *);

private:
	void restoreWindowState();
	void saveWindowState();
	void layoutWindow();
	void layoutToolBar();
	void layoutStatusBar();

	Ui::SyntroHAServerClass ui;

	int m_timer;

	QTabWidget *m_tabView;
	DeviceView *m_deviceView;
	TimerView *m_timerView;

    HAClient *m_client;
    InsteonDriver *m_insteonDriver;

	QAction *m_addAction;
	QAction *m_editAction;
	QAction *m_removeAction;

	QLabel *m_controlStatus;
	QLabel *m_insteonStatus;
};

#endif // SYNTROHAWINDOW_H
