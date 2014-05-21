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

#include <qheaderview.h>

#include "TimerView.h"
#include "InsteonDefs.h"

TimerView::TimerView(QWidget *parent)
	: QTableWidget(parent)
{
	layoutWindow();
}

void TimerView::updateTimerList(QList<InsteonTimer> timerList)
{
	int current = currentRow();

	setRowCount(0);
	addBlankRow();

	m_timerList = timerList;

	for (int i = 0; i < m_timerList.count(); i++)
		addTimer(m_timerList[i]);

	if (current >= 0 && current < (rowCount() - 1))
		selectRow(current);
}

void TimerView::addTimer(InsteonTimer timer)
{
	static QString dow[7] = { "S ", "M ", "T ", "W ", "Th ", "F ", "Sa" };
	QString mode;
	QString dowText;

	int row = rowCount() - 1;

	if (row == -1) {
		addBlankRow();
		row = 0;
	}

	insertRow(row);

	setItem(row, 0, new QTableWidgetItem(timer.name));
    if (timer.mode == INSTEON_TIMER_MODE_TOD)
        setItem(row, 1, new QTableWidgetItem(timer.time.toString("hh:mm")));
    else
        setItem(row, 1, new QTableWidgetItem("---"));

	for (int i = 0; i < 7; i++) {
		if (timer.daysOfWeek & (0x01 << i))
			dowText += dow[i];
	}

	setItem(row, 2, new QTableWidgetItem(dowText));

	if (timer.mode == INSTEON_TIMER_MODE_TOD)
		mode = "DOW";
	else if (timer.mode == INSTEON_TIMER_MODE_SUNRISE)
		mode = "Sunrise";
	else if (timer.mode == INSTEON_TIMER_MODE_SUNSET)
		mode = "Sunset";

	setItem(row, 3, new QTableWidgetItem(mode));
	setItem(row, 4, new QTableWidgetItem(QString::number(timer.deltaTime)));
	setItem(row, 5, new QTableWidgetItem(timer.randomMode ? "true" : "false"));
	setItem(row, 6, new QTableWidgetItem(timer.armed ? "true" : "false"));
	setItem(row, 7, new QTableWidgetItem(QString::number(timer.devices.count())));
	setItem(row, 8, new QTableWidgetItem(timer.triggerTime.toString("hh:mm")));
}

InsteonTimer TimerView::getCurrentTimer()
{
	InsteonTimer timer;

	int row = currentRow();

	if (row >= 0 && row < m_timerList.count())
		timer = m_timerList[row];

	return timer;
}

void TimerView::layoutWindow()
{
	QStringList headerLabels;

	headerLabels << "Name" << "Time" << "DOW" << "Mode" << "Delta" << "Random"
		<< "Armed" << "Device Count" << "Next Trigger Time";

	verticalHeader()->setDefaultSectionSize(20);
	verticalHeader()->setVisible(false);

	setColumnCount(headerLabels.count());
	setHorizontalHeaderLabels(headerLabels);
	horizontalHeader()->setStretchLastSection(true);

    setColumnWidth(0, 200);
    setColumnWidth(2, 150);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::SingleSelection);
	setEditTriggers(QAbstractItemView::NoEditTriggers);	

	addBlankRow();
}

void TimerView::addBlankRow()
{
	insertRow(0);

	for (int i = 0; i < columnCount(); i++)
		setItem(0, i, new QTableWidgetItem(""));
}
