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

#include <QRegExp>
#include <QSettings>

#include "dialogrule.h"
#include "ui_rule.h"

DialogRule::DialogRule(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RuleDialog)
{
    ui->setupUi(this);
}

DialogRule::DialogRule(const QString & find, const QString & replace, QWidget* parent) :
    QDialog(parent),
    ui(new Ui::RuleDialog)
{
    ui->setupUi(this);
    ui->lineEditFind->setText(find);
    ui->lineEditReplace->setText(replace);
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    setGeometry(qvariant_cast<QRect>(settings.value("ruleDialogSize", geometry())));
}

DialogRule::~DialogRule()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    settings.setValue("ruleDialogSize", geometry());
    delete ui;
}

RuleSet DialogRule::result(void) const
{
    return RuleSet(QRegExp(ui->lineEditFind->text()), ui->lineEditReplace->text());
}

void DialogRule::on_lineEditFind_textChanged(const QString &)
{
    ui->pushButtonAdd->setEnabled(ui->lineEditFind->text().size() && QRegExp(ui->lineEditFind->text()).isValid());
}
