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
#include <qserialportinfo.h>

#include "SerialDlg.h"

SerialDlg::SerialDlg(QWidget *parent, QString *portName) 
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	m_portName = portName;
	layoutWindow();

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(onOK()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

	setWindowTitle("Select serial port");
} 

void SerialDlg::onOK()
{
	if (m_serialPorts->currentText() == *m_portName)
		reject();

	*m_portName = m_serialPorts->currentText();
	accept();
}

void SerialDlg::layoutWindow()
{
	QVBoxLayout *vLayout = new QVBoxLayout();

	QFormLayout *formLayout = new QFormLayout();

	m_serialPorts = new QComboBox();

	QList<QSerialPortInfo> ports = QSerialPortInfo::availablePorts();
	foreach (QSerialPortInfo info, ports) {
		m_serialPorts->addItem(info.portName());
	}

    for (int i = 0; i < m_serialPorts->count(); i++) {
		if (*m_portName == m_serialPorts->itemText(i)) {
			m_serialPorts->setCurrentIndex(i);
			break;
		}
    }

    formLayout->addRow("Serial port", m_serialPorts);

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
