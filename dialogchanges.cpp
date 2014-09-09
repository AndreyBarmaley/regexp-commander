/***************************************************************************
 *   Copyright (C) 2014 by Andrey Afletdinov <afletdinov@gmail.com>        *
 *                                                                         *
 *   Part of the QRegexpCommander                                          *
 *   https://code.google.com/p/regexp-commander                            *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QFileInfo>
#include <QSettings>

#include "dialogchanges.h"
#include "ui_dialogchanges.h"

DialogChanges::DialogChanges(const QString & ruleset, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogChanges)
{
    ui->setupUi(this);
    ui->labelRuleset->setText(ruleset);

    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    setGeometry(qvariant_cast<QRect>(settings.value("changesDialogSize", geometry())));

    ui->treeWidgetChanges->setColumnWidth(0, qvariant_cast<int>(settings.value("changesColumnWidth0", 200)));
    ui->treeWidgetChanges->setColumnWidth(1, qvariant_cast<int>(settings.value("changesColumnWidth1", 200)));
    ui->treeWidgetChanges->setColumnWidth(2, qvariant_cast<int>(settings.value("changesColumnWidth2", 200)));
}

DialogChanges::~DialogChanges()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    settings.setValue("changesDialogSize", geometry());
    settings.setValue("changesColumnWidth0", ui->treeWidgetChanges->columnWidth(0));
    settings.setValue("changesColumnWidth1", ui->treeWidgetChanges->columnWidth(1));
    settings.setValue("changesColumnWidth2", ui->treeWidgetChanges->columnWidth(2));

    delete ui;
}

void DialogChanges::addContent(const QString & file, const QString & rev1, const QString & rev2)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << QFileInfo(file).fileName() << rev1 << rev2);
    item->setData(0, Qt::ToolTipRole, file);
    item->setData(1, Qt::ToolTipRole, rev1);
    item->setData(2, Qt::ToolTipRole, rev2);
    ui->treeWidgetChanges->addTopLevelItem(item);
}
