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

#include "SyntroHAController.h"
#include "SyntroAboutDlg.h"
#include "BasicSetupDlg.h"
#include "MainTable.h"
#include "SetInsteonLevelDlg.h"
#include "ChooseServerDlg.h"

#include <qheaderview.h>
#include <qboxlayout.h>
#include <qaction.h>
#include <QMessageBox>
#include <qdir.h>
#include <QStyleFactory>

SyntroHAController::SyntroHAController()
	: QMainWindow()
{
    m_logTag = "SyntroHAController";
    QApplication::setStyle(QStyleFactory::create("windows"));
	SyntroUtils::syntroAppInit();

    layoutWindow();
    restoreWindowState();
    initStatusBar();

    m_client = new ControllerClient();

	connect(m_client, SIGNAL(clientConnected()), this, SLOT(clientConnected()));
	connect(m_client, SIGNAL(clientClosed()), this, SLOT(clientClosed()));
    connect(m_client, SIGNAL(newUpdate(QJsonObject)), this, SLOT(newUpdate(QJsonObject)));
    connect(this, SIGNAL(setDeviceLevel(QJsonObject)), m_client, SLOT(setDeviceLevel(QJsonObject)));
    connect(this, SIGNAL(newServerName()), m_client, SLOT(newServerName()));
    connect(m_client, SIGNAL(serverName(QString)), this, SLOT(serverName(QString)));
    connect(this, SIGNAL(newSize(QSize)), m_table, SLOT(newSize(QSize)));

	connect(m_client, SIGNAL(dirResponse(QStringList)), this, SLOT(dirResponse(QStringList)));
	connect(this, SIGNAL(requestDir()), m_client, SLOT(requestDir()));

	m_client->resumeThread();

	m_statusTimer = startTimer(2000);

	setWindowTitle(QString("%1 - %2")
		.arg(SyntroUtils::getAppType())
		.arg(SyntroUtils::getAppName()));

    m_tapSeen = false;
	m_directoryTimer = startTimer(10000);
}

void SyntroHAController::closeEvent(QCloseEvent *)
{
 	killTimer(m_statusTimer);
	killTimer(m_directoryTimer);

	if (m_client) {
		m_client->exitThread(); 
		m_client = NULL;
	}

	saveWindowState();

	SyntroUtils::syntroAppExit();
}

void SyntroHAController::timerEvent(QTimerEvent *event)
{
	if (event->timerId() == m_directoryTimer) {
		emit requestDir();
	} else {
		m_controlStatus->setText(m_client->getLinkState());
	}
}

void SyntroHAController::dirResponse(QStringList directory)
{
	m_clientDirectory = directory;

	if (m_clientDirectory.length() > 0)
		m_chooseServer->setEnabled(true);
}


void SyntroHAController::serverName(QString serverName)
{
    m_serverName->setText(QString("Using automation server: ") + serverName);
}


void SyntroHAController::resizeEvent(QResizeEvent *)
{
    emit newSize(size());
}


void SyntroHAController::clientConnected()
{

}

void SyntroHAController::clientClosed()
{

}

void SyntroHAController::newUpdate(QJsonObject update)
{
    if (!update.contains(INSTEON_DEVICE_JSON_UPDATELIST))
        return;

    QJsonArray jsa = update[INSTEON_DEVICE_JSON_UPDATELIST].toArray();
    for (int i = 0; i < jsa.count(); i++) {
        QJsonObject params = jsa[i].toObject();

        InsteonDevice idev;
        if (!idev.read(params))
            continue;

        if (!m_map.contains(idev.deviceID)) {
            int row = m_table->rowCount();
            m_table->insertRow(row);
            m_table->setRowHeight(row, MAINTABLE_ROW_HEIGHT);
            QLabel *label = new QLabel();
            label->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            label->setIndent(20);
            m_table->setCellWidget(row, 0, label);

            QProgressBar *pb = new QProgressBar();
            pb->setMinimum(0);
            pb->setMaximum(255);
            pb->setAttribute(Qt::WA_TransparentForMouseEvents, true);
            m_table->setCellWidget(row, 1, pb);

            m_map.insert(idev.deviceID, idev);
            m_gridMap.insert(idev.deviceID, row);
        } else {
            m_map.insert(idev.deviceID, idev);
        }

        int gridSlot = m_gridMap.value(idev.deviceID);
        ((QLabel *)m_table->cellWidget(gridSlot, 0))->setText(idev.name);
        ((QProgressBar *)m_table->cellWidget(gridSlot, 1))->setValue(idev.currentLevel);
        m_table->update();
    }
}

void SyntroHAController::layoutWindow()
{
    QAction *action;

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    centralWidget->setContentsMargins(0, 0, 0, 0);

    m_toolBar = new QToolBar(this);
    addToolBar(Qt::TopToolBarArea, m_toolBar);
    m_toolBar->setObjectName("Toolbar");
    m_toolBar->setMinimumHeight(50);

    action = new QAction("Basic setup", this);
    connect(action, SIGNAL(triggered()), this, SLOT(onBasicSetup()));
    m_toolBar->addAction(action);

    m_toolBar->addSeparator();

    m_chooseServer = new QAction("Choose server", this);
	m_chooseServer->setEnabled(false);
    connect(m_chooseServer, SIGNAL(triggered()), this, SLOT(onChooseServer()));
    m_toolBar->addAction(m_chooseServer);

    m_toolBar->addSeparator();

    action = new QAction("About", this);
    connect(action, SIGNAL(triggered()), this, SLOT(onAbout()));
    m_toolBar->addAction(action);

    m_toolBar->addSeparator();

    action = new QAction("Exit", this);
    connect(action, SIGNAL(triggered()), this, SLOT(onExit()));
    m_toolBar->addAction(action);

    QVBoxLayout *vl = new QVBoxLayout();
    centralWidget->setLayout(vl);

    QHBoxLayout *hl = new QHBoxLayout();
    vl->addLayout(hl);

    m_table = new MainTable(this);
    m_table->setSelectionMode(QAbstractItemView::NoSelection);
    vl->addWidget(m_table);
    m_statusBar = new QStatusBar(this);
    setStatusBar(m_statusBar);

    connect(m_table, SIGNAL(cellClicked(int,int)), this, SLOT(cellClicked(int,int)));
    connect(m_table, SIGNAL(tapAndHold()), this, SLOT(tapAndHold()));
}

void SyntroHAController::tapAndHold()
{
    m_tapSeen = true;
}

void SyntroHAController::cellClicked(int row, int /* col */)
{   
 #if defined(Q_OS_ANDROID) || defined(Q_OS_IOS)
    if (!m_tapSeen)
        return;
#endif
    m_tapSeen = false;
    QString name = ((QLabel *)m_table->cellWidget(row, 0))->text();
    int level = ((QProgressBar *)m_table->cellWidget(row, 1))->value();
    SetInsteonLevelDlg dlg(this, name, level);

    if (dlg.exec() == QDialog::Accepted) {
        foreach(InsteonDevice idev, m_map) {
            if (idev.name == name) {
                QJsonArray jsa;
                QJsonObject jso;
                idev.newLevel = dlg.newLevel();
                idev.writeNewLevel(jso);
                jsa.append(jso);
                QJsonObject jsonNewLevel;
                jsonNewLevel[INSTEON_DEVICE_JSON_SETDEVICELEVEL] = jsa;
                emit setDeviceLevel(jsonNewLevel);
                return;
            }
        }
    }
}

void SyntroHAController::initStatusBar()
{
    m_controlStatus = new QLabel(this);
    m_controlStatus->setAlignment(Qt::AlignLeft);
    m_statusBar->addWidget(m_controlStatus, 1);
    m_serverName = new QLabel(this);
    m_serverName->setAlignment(Qt::AlignRight);
    m_statusBar->addWidget(m_serverName, 1);
}

void SyntroHAController::saveWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("Window");
	settings->setValue("Geometry", saveGeometry());
	settings->setValue("State", saveState());
	settings->endGroup();

	delete settings;
}

void SyntroHAController::restoreWindowState()
{
	QSettings *settings = SyntroUtils::getSettings();

	settings->beginGroup("Window");
	restoreGeometry(settings->value("Geometry").toByteArray());
	restoreState(settings->value("State").toByteArray());

	settings->endGroup();

	delete settings;
}

void SyntroHAController::onAbout()
{
	SyntroAbout dlg(this);
	dlg.exec();
}

void SyntroHAController::onBasicSetup()
{
	BasicSetupDlg dlg(this);
	dlg.exec();
}

void SyntroHAController::onExit()
{
    int ret = QMessageBox::question(this, "Exit SyntroHAController", "Are you sure you want to exit?",
                   QMessageBox::Ok | QMessageBox::Cancel, QMessageBox::Cancel);
    if (ret == QMessageBox::Ok)
        emit close();
}

void SyntroHAController::onChooseServer()
{
    ChooseServerDlg dlg(this, m_clientDirectory);

    if (dlg.exec() == QDialog::Accepted) {
        if (dlg.saveNewServerName()) {
            emit newServerName();
            while (m_table->rowCount() > 0)
                m_table->removeRow(0);
            m_gridMap.clear();
            m_map.clear();
        }
    }
}
