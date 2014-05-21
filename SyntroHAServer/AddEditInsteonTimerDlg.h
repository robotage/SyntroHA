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

#ifndef ADDEDITINSTEONTIMER_DLG_H
#define ADDEDITINSTEONTIMER_DLG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qcombobox.h>
#include <qtablewidget.h>

#include "InsteonTimer.h"
#include "InsteonDevice.h"
#include "InsteonDriver.h"

class AddEditInsteonTimerDlg : public QDialog
{
	Q_OBJECT

public:
    AddEditInsteonTimerDlg(QWidget *parent, InsteonDriver *insteonDriver, QList<InsteonTimer> timerList);
    AddEditInsteonTimerDlg(QWidget *parent, InsteonDriver *insteonDriver, InsteonTimer *timer);

	InsteonTimer newTimer();

public slots:
	void onOK();
	void onAddDevice();
	void onRemoveDevice();

private:
	void layoutWindow(InsteonTimer *timer);
	bool validateParams();
    void addDeviceRow(int deviceID, int level);

	bool m_editMode;
	QList<InsteonTimer> m_timerList;

	QLineEdit *m_name;
	QLineEdit *m_timeHour;
	QLineEdit *m_timeMinute;
	QCheckBox *m_daysOfWeek[7];
	QComboBox *m_mode;
	QCheckBox *m_randomMode;	
	QLineEdit *m_deltaTime;
	QCheckBox *m_armed;

	QTableWidget *m_deviceListWidget;

	QPushButton *m_addDevice;
	QPushButton *m_removeDevice;

	QPushButton *m_okButton;
	QPushButton *m_cancelButton;	

    InsteonDriver *m_insteonDriver;
};

#endif // ADDEDITINSTEONTIMER_DLG_H
