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

#include "InsteonDevice.h"

#include "AddEditInsteonDeviceDlg.h"

AddEditInsteonDeviceDlg::AddEditInsteonDeviceDlg(QWidget *parent, QList<InsteonDevice> deviceList) 
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	layoutWindow(NULL);
	
	m_deviceList = deviceList;

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setWindowTitle("Add Device");
} 

AddEditInsteonDeviceDlg::AddEditInsteonDeviceDlg(QWidget *parent, InsteonDevice *device) 
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	layoutWindow(device);

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	if (device)
		setWindowTitle("Edit Device");
	else
		setWindowTitle("Add Device");
} 

InsteonDevice AddEditInsteonDeviceDlg::newDevice()
{
	InsteonDevice device;
    bool ok;
	int cat, subcat;

    device.deviceID = (m_idEditA->text().toInt(&ok, 16) << 16) +
            (m_idEditB->text().toInt(&ok, 16) << 8) +
            (m_idEditC->text().toInt(&ok, 16) << 0);
	device.name = m_nameEdit->text();

	InsteonDevice::getCatFromType(m_type->currentText(), cat, subcat);

	device.deviceCat = cat;
	device.deviceSubcat = subcat;
	device.group = m_deviceGroupEdit->text().toInt();
	device.flags = m_flagsEdit->text().toInt();

	return device;
}

void AddEditInsteonDeviceDlg::onOK()
{
	if (validateParams())
		accept();
}

bool AddEditInsteonDeviceDlg::validateParams()
{
    bool ok;

	if (m_editMode)
		return true;

    unsigned int deviceID = (m_idEditA->text().toInt(&ok, 16) << 16) +
            (m_idEditB->text().toInt(&ok, 16) << 8) +
            (m_idEditC->text().toInt(&ok, 16) << 0);

	for (int i = 0; i < m_deviceList.count(); i++) {
		if (deviceID == m_deviceList.at(i).deviceID) {
            QMessageBox::warning(this, "Duplicate Device",
                                 QString("DeviceID %1 is already in use").arg(QString::number(deviceID, 16)));
			return false;
		}
	}

	return true;
}

void AddEditInsteonDeviceDlg::layoutWindow(InsteonDevice *device)
{
    QLabel *label;
    int index;

	QVBoxLayout *vLayout = new QVBoxLayout();

	QFormLayout *formLayout = new QFormLayout();

	m_type = new QComboBox();
	m_type->addItems(InsteonDevice::getDeviceTypes());

	if (device && device->deviceID > 0) {
        if ((index = m_type->findText(device->getTypeFromCat(device->deviceCat, device->deviceSubcat))) != -1)
            m_type->setCurrentIndex(index);
        m_idEditA = new QLineEdit(QString::number((device->deviceID >> 16) & 255, 16));
        m_idEditB = new QLineEdit(QString::number((device->deviceID >> 8) & 255, 16));
        m_idEditC = new QLineEdit(QString::number(device->deviceID & 255, 16));
        m_nameEdit = new QLineEdit(device->name);
		

		m_deviceGroupEdit = new QLineEdit(QString::number(device->group));	
		m_flagsEdit = new QLineEdit(QString::number(device->flags));

        m_idEditA->setEnabled(false);
        m_idEditB->setEnabled(false);
        m_idEditC->setEnabled(false);
        m_editMode = true;
    } else {
        m_idEditA = new QLineEdit("00");
        m_idEditB = new QLineEdit("00");
        m_idEditC = new QLineEdit("00");
        m_nameEdit = new QLineEdit;
        m_deviceGroupEdit = new QLineEdit("1");
        m_flagsEdit = new QLineEdit("266");
		m_editMode = false;
	}

	m_deviceGroupEdit->setValidator(new QIntValidator(0, 250));
	m_flagsEdit->setValidator(new QIntValidator(0, 255));

    QHBoxLayout *idBox = new QHBoxLayout();
    idBox->addWidget(m_idEditA);
    label = new QLabel(".");
    idBox->addWidget(label);
    idBox->addWidget(m_idEditB);
    label = new QLabel(".");
    idBox->addWidget(label);
    idBox->addWidget(m_idEditC);

    m_idEditA->setMaximumWidth(40);
    m_idEditB->setMaximumWidth(40);
    m_idEditC->setMaximumWidth(40);

    formLayout->addRow("Device ID (hex)", idBox);
	formLayout->addRow("Name", m_nameEdit);
	formLayout->addRow("Type", m_type);
	formLayout->addRow("Group", m_deviceGroupEdit);
	formLayout->addRow("Flags", m_flagsEdit);	

	vLayout->addLayout(formLayout);

	QHBoxLayout *hLayout = new QHBoxLayout();

	m_okButton = new QPushButton("OK");
	m_cancelButton = new QPushButton("Cancel");

	hLayout->addStretch();
	hLayout->addWidget(m_okButton);
	hLayout->addWidget(m_cancelButton);

	vLayout->addSpacing(20);
	vLayout->addLayout(hLayout);
	setLayout(vLayout);
};
