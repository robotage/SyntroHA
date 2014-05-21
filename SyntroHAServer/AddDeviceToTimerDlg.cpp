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

#include "AddDeviceToTimerDlg.h"
#include "InsteonDefs.h"

AddDeviceToTimerDlg::AddDeviceToTimerDlg(QWidget *parent, QList<InsteonDevice> deviceList)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
    m_deviceList = deviceList;
    layoutWindow();
    setWindowTitle("Add device");
}

int AddDeviceToTimerDlg::newDevice()
{
    return m_deviceID;
}

void AddDeviceToTimerDlg::onOK()
{
    int sel = m_deviceSelect->currentIndex();
    if (sel < 0) {
        reject();
        return;
    }

    m_deviceID = m_deviceSelect->itemData(sel).toInt();
    accept();
}

void AddDeviceToTimerDlg::layoutWindow()
{
	QHBoxLayout *hLayout;

	QVBoxLayout *mainLayout = new QVBoxLayout();

    m_deviceSelect = new QComboBox;
    m_deviceSelect->setMinimumWidth(250);

    for (int i = 0; i < m_deviceList.count(); i++)
        m_deviceSelect->addItem(InsteonDevice::deviceIDToString(m_deviceList[i].deviceID) + ":" + m_deviceList[i].name,
                                m_deviceList[i].deviceID);

    mainLayout->addWidget(m_deviceSelect);

	hLayout = new QHBoxLayout();

	m_okButton = new QPushButton("OK");
	m_cancelButton = new QPushButton("Cancel");

	hLayout->addStretch();
	hLayout->addWidget(m_okButton);
	hLayout->addWidget(m_cancelButton);

	mainLayout->addSpacing(12);
	mainLayout->addLayout(hLayout);
	setLayout(mainLayout);

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
};
