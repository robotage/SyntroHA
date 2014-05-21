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
#include <qformlayout.h>

#include "ChooseServerDlg.h"
#include "SyntroUtils.h"
#include "DirectoryEntry.h"
#include "SyntroHAController.h"

ChooseServerDlg::ChooseServerDlg(QWidget *parent, const QStringList& directory)
	: QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint)
{
	DirectoryEntry de;
	QString servicePath;
	QString streamName;
	QString streamSource;

	for (int i = 0; i < directory.count(); i++) {
		de.setLine(directory.at(i));

		if (!de.isValid())
			continue;
	
		QStringList services = de.multicastServices();

		for (int i = 0; i < services.count(); i++) {
			servicePath = de.appName() + SYNTRO_SERVICEPATH_SEP + services.at(i);

			SyntroUtils::removeStreamNameFromPath(servicePath, streamSource, streamName);
 
			if (m_servers.contains(streamSource))
				continue;

			if (streamName == SYNTRO_STREAMNAME_HOMEAUTOMATION)
				m_servers.append(streamSource);
		}
	}		

	layoutWindow();
    setWindowTitle("Choose Automation Server");
    connect(m_buttons, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttons, SIGNAL(rejected()), this, SLOT(reject()));
}


ChooseServerDlg::~ChooseServerDlg()
{
	delete m_validator;
}

bool ChooseServerDlg::saveNewServerName()
{
	QSettings *settings = SyntroUtils::getSettings();

    if (m_serverName->currentText() == settings->value(SYNTROHACONTROLLER_SERVER_NAME).toString()) {
        delete settings;
        return false;
    }

    settings->setValue(SYNTROHACONTROLLER_SERVER_NAME, m_serverName->currentText());
    settings->sync();
	delete settings;

    return true;
}

void ChooseServerDlg::layoutWindow()
{
	QSettings *settings = SyntroUtils::getSettings();

    setModal(true);

	QVBoxLayout *centralLayout = new QVBoxLayout(this);
	centralLayout->setSpacing(20);
	centralLayout->setContentsMargins(11, 11, 11, 11);

#if defined(Q_OS_IOS) || defined(Q_OS_ANDROID)
    QLabel *header = new QLabel("Choose Automation Server");
    header->setAlignment(Qt::AlignHCenter);
    centralLayout->addWidget(header);
#endif

	QFormLayout *formLayout = new QFormLayout();
	formLayout->setSpacing(16);
	formLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);

	m_serverName = new QComboBox();
	m_serverName->setEditable(false);

	int currentIndex = -1;
	for (int i = 0; i < m_servers.count(); i++) {
		m_serverName->addItem(m_servers.at(i));
		if (m_servers.at(i) == settings->value(SYNTROHACONTROLLER_SERVER_NAME).toString())
			currentIndex = i;
	}
	if (currentIndex != -1)
		m_serverName->setCurrentIndex(currentIndex);

    m_serverName->setToolTip("The app name of the server to use.");
    m_serverName->setMinimumWidth(200);
    formLayout->addRow(tr("Server name:"), m_serverName);
	m_validator = new ServiceNameValidator();

	centralLayout->addLayout(formLayout);

	m_buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
	m_buttons->setCenterButtons(true);

	centralLayout->addWidget(m_buttons);
}
