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
#include <qvalidator.h>

#include "InsteonDevice.h"

#include "SetInsteonLevelDlg.h"

SetInsteonLevelDlg::SetInsteonLevelDlg(QWidget *parent, int deviceID, QString name, int level) 
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	layoutWindow(deviceID, name, level);

	connect(m_okButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(m_cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(m_on, SIGNAL(clicked(bool)), this, SLOT(onClicked(bool)));
    connect(m_off, SIGNAL(clicked(bool)), this, SLOT(offClicked(bool)));

	setWindowTitle("Set Level");
} 

int SetInsteonLevelDlg::newLevel()
{
    return m_level->value();
}

void SetInsteonLevelDlg::onClicked(bool)
{
    m_level->setValue(255);
}

void SetInsteonLevelDlg::offClicked(bool)
{
    m_level->setValue(0);
}

void SetInsteonLevelDlg::layoutWindow(int deviceID, QString name, int level)
{
	QVBoxLayout *vLayout = new QVBoxLayout();

	QFormLayout *formLayout = new QFormLayout();

	formLayout->addRow("Device ID", new QLabel(InsteonDevice::deviceIDToString(deviceID)));
	formLayout->addRow("Name", new QLabel(name));

    QHBoxLayout *levelBox = new QHBoxLayout();
    m_on = new QPushButton("On");
    m_off = new QPushButton("Off");
    m_level = new QSlider(Qt::Horizontal);
    m_level->setRange(0, 255);
    m_level->setTickInterval(15);
    m_level->setTickPosition(QSlider::TicksBelow);
    if ((level >= 0) && (level <= 255))
        m_level->setValue(level);
    else
        m_level->setValue(0);
    m_level->setMinimumWidth(200);
    levelBox->addWidget(m_off);
    levelBox->addWidget(m_level);
    levelBox->addWidget(m_on);

    formLayout->addRow("Level", levelBox);

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
