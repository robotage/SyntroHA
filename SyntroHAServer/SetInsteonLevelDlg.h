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

#ifndef SETINSTEONLEVEL_DLG_H
#define SETINSTEONLEVEL_DLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qslider.h>

class SetInsteonLevelDlg : public QDialog
{
	Q_OBJECT

public:
	SetInsteonLevelDlg(QWidget *parent, int deviceID, QString name, int level);

	int newLevel();

public slots:
    void onClicked(bool);
    void offClicked(bool);


private:
	void layoutWindow(int deviceID, QString name, int level);

    QSlider *m_level;
    QPushButton *m_on;
    QPushButton *m_off;

	QPushButton *m_okButton;
	QPushButton *m_cancelButton;	
};

#endif // SETINSTEONLEVEL_DLG_H
