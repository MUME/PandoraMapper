/*
 *  Pandora MUME mapper
 *
 *  Copyright (C) 2000-2009  Azazello
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License
 *  as published by the Free Software Foundation; either version 2
 *  of the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef FINDDIALOG_H
#define FINDDIALOG_H
#include <QDialog>
#include "ui_finddialog.h"

class FindDialog : public QDialog, public Ui::FindDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);

private:
    void adjustResultTable();

private slots:
    void on_lineEdit_textChanged();
    void findClicked();
    void enableFindButton(const QString &text);
    void itemDoubleClicked(QTreeWidgetItem *item);
};
#endif
