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

#ifndef CHOOSESERVERDLG_H
#define CHOOSESERVERDLG_H

#include <QDialog>
#include <qsettings.h>
#include <qlineedit.h>
#include <qdialogbuttonbox.h>
#include <qmessagebox.h>
#include <qcombobox.h>

#include "SyntroValidators.h"

#include "SyntroDefs.h"

class ChooseServerDlg : public QDialog
{
	Q_OBJECT

public:
    ChooseServerDlg(QWidget *parent, const QStringList& directory);
    ~ChooseServerDlg();

    bool saveNewServerName();

private:
	void layoutWindow();

	QStringList m_servers;
    QComboBox *m_serverName;
	QDialogButtonBox *m_buttons;
	ServiceNameValidator *m_validator;
};

#endif // CHOOSESERVERDLG_H
