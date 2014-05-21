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

#include <qboxlayout.h>

#include "SyntroHAWindow.h"
#include "SyntroHAServer.h"
#include "HAClient.h"
#include "InsteonDriver.h"
#include "BasicSetupDlg.h"
#include "SyntroAboutDlg.h"
#include "AddEditInsteonDeviceDlg.h"
#include "SetInsteonLevelDlg.h"
#include "AddEditInsteonTimerDlg.h"
#include "SerialDlg.h"

SyntroHAWindow::SyntroHAWindow()
    : QMainWindow()
{
	ui.setupUi(this);

	qRegisterMetaType<InsteonDevice>("InsteonDevice");
	qRegisterMetaType<InsteonTimer>("InsteonTimer");
	qRegisterMetaType<InsteonTimerDevice>("InsteonTimerDevice");

	layoutWindow();
	layoutToolBar();
	layoutStatusBar();
	restoreWindowState();

	connect(ui.actionExit, SIGNAL(triggered()), this, SLOT(close()));
	connect(ui.actionBasicSetup, SIGNAL(triggered()), this, SLOT(onBasicSetup()));
	connect(ui.actionSerialPort, SIGNAL(triggered()), this, SLOT(onSerialPortSetup()));
	connect(ui.actionAbout, SIGNAL(triggered()), this, SLOT(onAbout()));

	connect(m_tabView, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));
	connect(ui.actionAddDevice, SIGNAL(triggered()), this, SLOT(onAddDevice()));
	connect(ui.actionEditDevice, SIGNAL(triggered()), this, SLOT(onEditDevice()));
	connect(ui.actionRemoveDevice, SIGNAL(triggered()), this, SLOT(onRemoveDevice()));
	connect(ui.actionAddTimer, SIGNAL(triggered()), this, SLOT(onAddTimer()));
	connect(ui.actionEditTimer, SIGNAL(triggered()), this, SLOT(onEditTimer()));
	connect(ui.actionRemoveTimer, SIGNAL(triggered()), this, SLOT(onRemoveTimer()));

	connect(m_deviceView, SIGNAL(itemSelectionChanged()), this, SLOT(deviceSelectionChanged()));
	connect(m_timerView, SIGNAL(itemSelectionChanged()), this, SLOT(timerSelectionChanged()));

	connect(m_deviceView, SIGNAL(doubleClickEvent()), this, SLOT(deviceDoubleClickEvent()));

   SyntroUtils::syntroAppInit();

    m_client = new HAClient(this);
    m_insteonDriver = new InsteonDriver();

	connect(this, SIGNAL(addDevice(InsteonDevice)), m_insteonDriver, SLOT(addDevice(InsteonDevice)));
	connect(this, SIGNAL(removeDevice(int)), m_insteonDriver, SLOT(removeDevice(int)));
    connect(this, SIGNAL(setDeviceLevel(int, int)), m_insteonDriver, SLOT(setDeviceLevel(int, int)));
    connect(m_client, SIGNAL(setDeviceLevel(int, int)), m_insteonDriver, SLOT(setDeviceLevel(int, int)));

	connect(this, SIGNAL(addTimer(InsteonTimer)), m_insteonDriver, SLOT(addTimer(InsteonTimer)));
	connect(this, SIGNAL(updateTimer(InsteonTimer)), m_insteonDriver, SLOT(updateTimer(InsteonTimer)));
	connect(this, SIGNAL(removeTimer(QString)), m_insteonDriver, SLOT(removeTimer(QString)));

	connect(this, SIGNAL(newPortName(QString)), m_insteonDriver, SLOT(newPortName(QString)));
 
    connect(m_insteonDriver, SIGNAL(newSyntroUpdate(QJsonObject)), m_client, SLOT(newSyntroUpdate(QJsonObject)));

    m_client->resumeThread();
    m_insteonDriver->resumeThread();

	tabChanged(0);

	m_timer = startTimer(1000);
}

void SyntroHAWindow::closeEvent(QCloseEvent *)
{
	if (m_timer) {
		killTimer(m_timer);
		m_timer = 0;
	}

	if (m_client) {
		m_client->exitThread();
		m_client = NULL;
	}

	if (m_insteonDriver) {
		m_insteonDriver->exitThread();
		m_insteonDriver = NULL;
	}

	saveWindowState();

	SyntroUtils::syntroAppExit();
}

void SyntroHAWindow::timerEvent(QTimerEvent *)
{
	QString state = m_insteonDriver->getState();

	if (state == "active") {	
		QList<InsteonDevice> deviceList = m_insteonDriver->getDeviceList();
		m_deviceView->updateDeviceList(deviceList);

		QList<InsteonTimer> timerList = m_insteonDriver->getTimerList();
		m_timerView->updateTimerList(timerList);
	}

	m_controlStatus->setText(QString("Control: ") + m_client->getLinkState());
	m_insteonStatus->setText(QString("Insteon: ") + state);
}

void SyntroHAWindow::tabChanged(int index)
{
	if (index == 0) {
		m_timerView->selectionModel()->clearSelection();
		ui.actionAddDevice->setEnabled(true);
		ui.actionAddTimer->setEnabled(false);
	}
	else {
		m_deviceView->selectionModel()->clearSelection();
		ui.actionAddDevice->setEnabled(false);
		ui.actionAddTimer->setEnabled(true);
	}

	ui.actionEditDevice->setEnabled(false);
	ui.actionRemoveDevice->setEnabled(false);
	ui.actionEditTimer->setEnabled(false);
	ui.actionRemoveTimer->setEnabled(false);
	m_editAction->setEnabled(false);
	m_removeAction->setEnabled(false);
}

void SyntroHAWindow::onAdd()
{
	if (m_tabView->currentIndex() == 0)
		onAddDevice();
	else
		onAddTimer();
}

void SyntroHAWindow::onEdit()
{
	if (m_tabView->currentIndex() == 0)
		onEditDevice();
	else
		onEditTimer();
}

void SyntroHAWindow::onRemove()
{
	if (m_tabView->currentIndex() == 0)
		onRemoveDevice();
	else
		onRemoveTimer();
}

void SyntroHAWindow::onAddDevice()
{
	AddEditInsteonDeviceDlg dlg(this, m_insteonDriver->getDeviceList());

	if (dlg.exec() == QDialog::Accepted) {
		InsteonDevice device = dlg.newDevice();
		emit addDevice(device);
	}		
}

void SyntroHAWindow::onEditDevice()
{
	InsteonDevice device = m_deviceView->getCurrentDevice();

	AddEditInsteonDeviceDlg dlg(this, &device);

	if (dlg.exec() == QDialog::Accepted) {
		device = dlg.newDevice();
		emit addDevice(device);
	}		
}

void SyntroHAWindow::onRemoveDevice()
{
	InsteonDevice device = m_deviceView->getCurrentDevice();

    int ret = QMessageBox::warning(this, "Device", QString("Are you sure you want to delete ") + device.name + "?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if (ret == QMessageBox::Ok)
        emit removeDevice(device.deviceID);
}

void SyntroHAWindow::onAddTimer()
{
    AddEditInsteonTimerDlg dlg(this, m_insteonDriver, m_insteonDriver->getTimerList());

	if (dlg.exec() == QDialog::Accepted) {
		InsteonTimer timer = dlg.newTimer();
		emit addTimer(timer);
	}
}

void SyntroHAWindow::onEditTimer()
{
	InsteonTimer timer = m_timerView->getCurrentTimer();

    AddEditInsteonTimerDlg dlg(this, m_insteonDriver, &timer);

	if (dlg.exec() == QDialog::Accepted) {
		InsteonTimer timer = dlg.newTimer();
		emit updateTimer(timer);
	}
}

void SyntroHAWindow::onRemoveTimer()
{
	InsteonTimer timer = m_timerView->getCurrentTimer();

    int ret = QMessageBox::warning(this, "Timer", QString("Are you sure you want to delete ") + timer.name + "?",
                                   QMessageBox::Ok | QMessageBox::Cancel);

    if (ret == QMessageBox::Ok)
        emit removeTimer(timer.name);
}

void SyntroHAWindow::deviceSelectionChanged()
{
	if (m_tabView->currentIndex() != 0)
		return;

	int row = m_deviceView->currentRow();

    if ((row < 0) || (row >= m_deviceView->rowCount())) {
		ui.actionEditDevice->setEnabled(false);
		ui.actionRemoveDevice->setEnabled(false);
		m_editAction->setEnabled(false);
		m_removeAction->setEnabled(false);
    } else {
		ui.actionEditDevice->setEnabled(true);
		ui.actionRemoveDevice->setEnabled(true);
		m_editAction->setEnabled(true);
		m_removeAction->setEnabled(true);
	}
}

void SyntroHAWindow::timerSelectionChanged()
{
	if (m_tabView->currentIndex() != 1)
		return;

	int row = m_timerView->currentRow();

    if ((row < 0) || (row >= m_timerView->rowCount())) {
        ui.actionEditTimer->setEnabled(false);
		ui.actionRemoveTimer->setEnabled(false);
		m_editAction->setEnabled(false);
		m_removeAction->setEnabled(false);
	}
	else {
		ui.actionEditTimer->setEnabled(true);
		ui.actionRemoveTimer->setEnabled(true);
		m_editAction->setEnabled(true);
		m_removeAction->setEnabled(true);
	}
}

void SyntroHAWindow::deviceDoubleClickEvent()
{
	InsteonDevice device = m_deviceView->getCurrentDevice();

	if (device.deviceID == 0)
		return;

	SetInsteonLevelDlg dlg(this, device.deviceID, device.name, device.currentLevel);

	if (dlg.exec() == QDialog::Accepted)
		emit setDeviceLevel(device.deviceID, dlg.newLevel());
}

void SyntroHAWindow::onBasicSetup()
{
	BasicSetupDlg dlg(this);
	dlg.exec();
}

void SyntroHAWindow::onAbout()
{
	SyntroAbout dlg;
	dlg.exec();
}

void SyntroHAWindow::onSerialPortSetup() {
    QString portName = m_insteonDriver->getPortName();

	SerialDlg dlg(this, &portName);
	if (dlg.exec() == QDialog::Accepted)
		emit newPortName(portName);
}

void SyntroHAWindow::layoutWindow()
{
	m_tabView = new QTabWidget(this);

	m_deviceView = new DeviceView(this);
	m_tabView->addTab(m_deviceView, "Devices");

	m_timerView = new TimerView(this);
	m_tabView->addTab(m_timerView, "Timers");

	QVBoxLayout *vLayout = new QVBoxLayout();
	vLayout->setContentsMargins(0, 0, 0, 0);
	vLayout->setSpacing(0);
	vLayout->addWidget(m_tabView);

	centralWidget()->setLayout(vLayout);

	setMinimumSize(1000, 400);
}

void SyntroHAWindow::layoutToolBar()
{
	m_addAction = new QAction("Add", this);
	connect(m_addAction, SIGNAL(triggered()), this, SLOT(onAdd()));
	
	m_editAction = new QAction("Edit", this);
	connect(m_editAction, SIGNAL(triggered()), this, SLOT(onEdit()));

	m_removeAction = new QAction("Remove", this);
	connect(m_removeAction, SIGNAL(triggered()), this, SLOT(onRemove()));
	
	ui.mainToolBar->addAction(m_addAction);
	ui.mainToolBar->addAction(m_editAction);
	ui.mainToolBar->addAction(m_removeAction);
}

void SyntroHAWindow::layoutStatusBar()
{
	m_controlStatus = new QLabel;
	m_controlStatus->setAlignment(Qt::AlignLeft);
	ui.statusbar->addWidget(m_controlStatus);

	m_insteonStatus = new QLabel;
	m_insteonStatus->setAlignment(Qt::AlignRight);
	ui.statusbar->addWidget(m_insteonStatus, 1);
}

void SyntroHAWindow::restoreWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("MainWindow");
	restoreGeometry(settings->value("Geometry").toByteArray());
	restoreState(settings->value("State").toByteArray());
	settings->endGroup();

	delete settings;
}

void SyntroHAWindow::saveWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("MainWindow");
	settings->setValue("Geometry", saveGeometry());
	settings->setValue("State", saveState());
	settings->endGroup();

	delete settings;
	
}
