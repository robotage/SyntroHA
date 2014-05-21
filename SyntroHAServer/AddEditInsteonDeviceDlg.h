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

#ifndef ADDEDITINSTEONDEVICE_DLG_H
#define ADDEDITINSTEONDEVICE_DLG_H

#include <qdialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qcombobox.h>

#include "InsteonDevice.h"

class AddEditInsteonDeviceDlg : public QDialog
{
	Q_OBJECT

public:
	AddEditInsteonDeviceDlg(QWidget *parent, QList<InsteonDevice> deviceList);
	AddEditInsteonDeviceDlg(QWidget *parent, InsteonDevice *device);

	InsteonDevice newDevice();

public slots:
	void onOK();

private:
	void layoutWindow(InsteonDevice *device);
	bool validateParams();

	bool m_editMode;
	QList<InsteonDevice> m_deviceList;

    QLineEdit *m_idEditA;
    QLineEdit *m_idEditB;
    QLineEdit *m_idEditC;
    QComboBox *m_type;
	QLineEdit *m_deviceGroupEdit;
	QLineEdit *m_flagsEdit;
	QLineEdit *m_nameEdit;

	QPushButton *m_okButton;
	QPushButton *m_cancelButton;	
};

#endif // ADDEDITINSTEONDEVICE_DLG_H
