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

#ifndef ADDDEVICETOTIMERDLG_H
#define ADDDEVICETOTIMERDLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qcombobox.h>

#include "InsteonDevice.h"

class AddDeviceToTimerDlg : public QDialog
{
	Q_OBJECT

public:
    AddDeviceToTimerDlg(QWidget *parent, QList<InsteonDevice> deviceList);

    int newDevice();

public slots:
	void onOK();

private:
    void layoutWindow();

    QList<InsteonDevice> m_deviceList;

    QComboBox *m_deviceSelect;

	QPushButton *m_okButton;
	QPushButton *m_cancelButton;	

    int m_deviceID;
};

#endif // ADDDEVICETOTIMERDLG_H
