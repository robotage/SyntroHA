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

#include <qlabel.h>
#include <qboxlayout.h>
#include <qformlayout.h>
#include <qmessagebox.h>
#include <qvalidator.h>
#include <qgroupbox.h>
#include <qheaderview.h>

#include "AddEditInsteonTimerDlg.h"
#include "AddDeviceToTimerDlg.h"
#include "InsteonDefs.h"

AddEditInsteonTimerDlg::AddEditInsteonTimerDlg(QWidget *parent, InsteonDriver *insteonDriver, QList<InsteonTimer> timerList)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	m_timerList = timerList;
    m_insteonDriver = insteonDriver;

	layoutWindow(NULL);

	m_editMode = false;

	setWindowTitle("Add Timer");
}

AddEditInsteonTimerDlg::AddEditInsteonTimerDlg(QWidget *parent, InsteonDriver *insteonDriver, InsteonTimer *timer)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    m_insteonDriver = insteonDriver;
    layoutWindow(timer);

	if (timer)
		setWindowTitle("Edit Timer");
	else
		setWindowTitle("Add Timer");
} 

InsteonTimer AddEditInsteonTimerDlg::newTimer()
{
	InsteonTimer timer;
	InsteonTimerDevice timerDevice;
	bool ok;

	timer.name = m_name->text();

	int hour = m_timeHour->text().toInt();
	int minute = m_timeMinute->text().toInt();

	timer.time.setHMS(hour, minute, 0);

	for (int i = 0; i < 7; i++) {
		if (m_daysOfWeek[i]->isChecked())
			timer.daysOfWeek |= (0x01 << i);
	}

	timer.mode = m_mode->currentIndex();	
	timer.randomMode = m_randomMode->isChecked();
	timer.deltaTime = m_deltaTime->text().toInt();
	timer.armed = m_armed->isChecked();

	for (int i = 0; i < m_deviceListWidget->rowCount(); i++) {
		timerDevice.deviceID = m_deviceListWidget->item(i, 0)->text().toInt(&ok, 16);
        timerDevice.level = m_deviceListWidget->item(i, 2)->text().toInt();
		timer.devices.append(timerDevice);
	}

	return timer;
}

void AddEditInsteonTimerDlg::onOK()
{
	if (validateParams())
		accept();
}

bool AddEditInsteonTimerDlg::validateParams()
{
	bool ok;
	int deviceID;
	int level;

	QString name = m_name->text();
	if (!m_editMode) {

		if (name.length() == 0) {
			QMessageBox::warning(this, "Missing name", "Timer name cannot be blank");
			return false;
		}

		for (int i = 0; i < m_timerList.count(); i++) {
			if (name == m_timerList.at(i).name) {
				QMessageBox::warning(this, "Duplicate Timer", QString("Timer named %1 is already in use").arg(name));
				return false;
			}
		}
	}

	for (int i = 0; i < m_deviceListWidget->rowCount(); i++) {
		QString s = m_deviceListWidget->item(i, 0)->text();

		if (s.length() == 0)
			continue;

		deviceID = s.toInt(&ok, 16);

		if (!ok || deviceID <= 0 || deviceID > 0xffffff) {
			QMessageBox::warning(this, "Invalid device ID", QString("DeviceID %1 is not valid").arg(QString(deviceID, 16)));
			return false;
		}
			
        s = m_deviceListWidget->item(i, 2)->text();

		if (s.length() == 0)
			continue;

		level = s.toInt(&ok);

		if (!ok || level < 0 || level > 255) {
			QMessageBox::warning(this, "Invalid device level", QString("A level of %1 is not valid").arg(level));
			return false;
		}			
	}

	return true;
}

void AddEditInsteonTimerDlg::onAddDevice()
{
    int index;
    unsigned int deviceID;
    bool ok;

    QList<InsteonDevice> deviceList = m_insteonDriver->getDeviceList();
    QList<InsteonDevice> filtered;

    //  filter list to only offer devices not already selected

    for (int i = 0; i < deviceList.count(); i++) {
        for (index = 0; index < m_deviceListWidget->rowCount(); index++) {
            deviceID = m_deviceListWidget->item(index, 0)->text().toInt(&ok, 16);
            if (deviceID == deviceList.at(i).deviceID)
                break;
        }
        if (index != m_deviceListWidget->rowCount())
            continue;
        filtered.append(deviceList.at(i));
    }

    AddDeviceToTimerDlg dlg(this, filtered);
    if (dlg.exec() != QDialog::Accepted)
        return;
    addDeviceRow(dlg.newDevice(), 0);
}

void AddEditInsteonTimerDlg::onRemoveDevice()
{
	int row = m_deviceListWidget->currentRow();

	if (row < 0 || row >= m_deviceListWidget->rowCount())
		return;

	m_deviceListWidget->removeRow(row);
}

void AddEditInsteonTimerDlg::layoutWindow(InsteonTimer *timer)
{
	QVBoxLayout *vLayout;
	QHBoxLayout *hLayout;
	QFormLayout *formLayout;
	QGroupBox *group;

	QVBoxLayout *mainLayout = new QVBoxLayout();

	m_name = new QLineEdit;

	m_timeHour = new QLineEdit("0");
	m_timeHour->setValidator(new QIntValidator(0, 23));
	m_timeHour->setMaximumWidth(40);
	m_timeMinute = new QLineEdit("0");
	m_timeMinute->setValidator(new QIntValidator(0, 59));
	m_timeMinute->setMaximumWidth(40);

	m_daysOfWeek[0] = new QCheckBox("S");
	m_daysOfWeek[1] = new QCheckBox("M");
	m_daysOfWeek[2] = new QCheckBox("T");
	m_daysOfWeek[3] = new QCheckBox("W");
	m_daysOfWeek[4] = new QCheckBox("Th");
	m_daysOfWeek[5] = new QCheckBox("F");
	m_daysOfWeek[6] = new QCheckBox("Sa");

	m_mode = new QComboBox;
	m_mode->insertItem(0, "Time Of Day");
	m_mode->insertItem(1, "Sunrise");
	m_mode->insertItem(2, "Sunset");
	m_mode->setCurrentIndex(0);
    m_mode->setMinimumWidth(150);
    m_mode->setMaximumWidth(150);

	m_randomMode = new QCheckBox("Randomize Delta");

	m_deltaTime = new QLineEdit("0");
	m_deltaTime->setValidator(new QIntValidator(0, 360));
	m_deltaTime->setMaximumWidth(40);

	m_armed = new QCheckBox("Armed");
	
	if (timer) {
		m_editMode = true;
		
		m_name->setText(timer->name);
		m_name->setEnabled(false);

		m_timeHour->setText(QString::number(timer->time.hour()));
		m_timeMinute->setText(QString::number(timer->time.minute()));

		quint32 mask = 0x01;

		for (int i = 0; i < 7; i++) {
			if (mask & timer->daysOfWeek)
				m_daysOfWeek[i]->setChecked(true);

			mask <<= 1;
		}

		if (timer->mode == INSTEON_TIMER_MODE_TOD)
			m_mode->setCurrentIndex(0);
		else if (timer->mode == INSTEON_TIMER_MODE_SUNRISE)
			m_mode->setCurrentIndex(1);
		else if (timer->mode == INSTEON_TIMER_MODE_SUNSET)
			m_mode->setCurrentIndex(2);
		
		m_randomMode->setChecked(timer->randomMode);

		if (timer->deltaTime >= 0 && timer->deltaTime <= 360)
			m_deltaTime->setText(QString::number(timer->deltaTime));

		m_armed->setChecked(timer->armed);
    } else {
		m_editMode = false;

		for (int i = 0; i < 7; i++)
			m_daysOfWeek[i]->setChecked(true);

		m_armed->setChecked(true);
	}

	formLayout = new QFormLayout;
	formLayout->addRow("Name", m_name);
	mainLayout->addLayout(formLayout);

	group = new QGroupBox("Time");
	hLayout = new QHBoxLayout();

	formLayout = new QFormLayout;
	formLayout->addRow("Hour", m_timeHour);
	hLayout->addLayout(formLayout);

	formLayout = new QFormLayout;
	formLayout->addRow("Minute", m_timeMinute);
	hLayout->addLayout(formLayout);

	hLayout->addStretch();

	group->setLayout(hLayout);
	mainLayout->addWidget(group);

	group = new QGroupBox("Days Of Week");
	hLayout = new QHBoxLayout;

	for (int i = 0; i < 7; i++)
		hLayout->addWidget(m_daysOfWeek[i]);
	
	hLayout->addStretch();
	group->setLayout(hLayout);
	mainLayout->addWidget(group);

	group = new QGroupBox("Operation");
	vLayout = new QVBoxLayout;

	formLayout = new QFormLayout;
	formLayout->addRow("Mode", m_mode);
	vLayout->addLayout(formLayout);
	
	hLayout = new QHBoxLayout;
	formLayout = new QFormLayout;
	formLayout->addRow("Delta (minutes)", m_deltaTime);
	hLayout->addLayout(formLayout);
	hLayout->addWidget(m_randomMode);
	hLayout->addStretch();
	vLayout->addLayout(hLayout);

	vLayout->addWidget(m_armed);

	group->setLayout(vLayout);
	mainLayout->addWidget(group);


	group = new QGroupBox("Devices to Control");
	vLayout = new QVBoxLayout;

	m_deviceListWidget = new QTableWidget(this);

	m_deviceListWidget->verticalHeader()->setDefaultSectionSize(20);
	m_deviceListWidget->verticalHeader()->setVisible(false);

    m_deviceListWidget->setColumnCount(3);
    m_deviceListWidget->setHorizontalHeaderLabels(QStringList() << "DeviceID" << "Name" << "Level");
	m_deviceListWidget->horizontalHeader()->setStretchLastSection(true);
    m_deviceListWidget->setColumnWidth(1, 150);

	m_deviceListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	if (timer) {
		for (int i = 0; i < timer->devices.count(); i++) {
            addDeviceRow(timer->devices[i].deviceID, timer->devices[i].level);
        }
	}

	vLayout->addWidget(m_deviceListWidget);

	hLayout = new QHBoxLayout;
	m_addDevice = new QPushButton("Add Device");
	m_removeDevice = new QPushButton("Remove Device");

	hLayout->addStretch();
	hLayout->addWidget(m_addDevice);
	hLayout->addWidget(m_removeDevice);
	hLayout->addStretch();
	vLayout->addLayout(hLayout);

	group->setLayout(vLayout);
	mainLayout->addWidget(group);

	hLayout = new QHBoxLayout();

	m_okButton = new QPushButton("OK");
	m_cancelButton = new QPushButton("Cancel");

	hLayout->addStretch();
	hLayout->addWidget(m_okButton);
	hLayout->addWidget(m_cancelButton);

	mainLayout->addSpacing(12);
	mainLayout->addLayout(hLayout);
	setLayout(mainLayout);

	connect(m_addDevice, SIGNAL(clicked()), this, SLOT(onAddDevice()));
	connect(m_removeDevice, SIGNAL(clicked()), this, SLOT(onRemoveDevice()));

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setMinimumSize(500, 400);
};

void AddEditInsteonTimerDlg::addDeviceRow(int deviceID, int level)
{
    int row = m_deviceListWidget->rowCount();
    m_deviceListWidget->insertRow(row);
    QTableWidgetItem *dev = new QTableWidgetItem(QString::number(deviceID, 16));
    dev->setFlags(dev->flags() & ~Qt::ItemIsEditable);
    m_deviceListWidget->setItem(row, 0, dev);

    QTableWidgetItem *name = new QTableWidgetItem(m_insteonDriver->getDeviceName(deviceID));
    name->setFlags(dev->flags() & ~Qt::ItemIsEditable);
    m_deviceListWidget->setItem(row, 1, name);
    m_deviceListWidget->setItem(row, 2, new QTableWidgetItem(QString::number(level)));

}
