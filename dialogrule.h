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

#ifndef RULE_H
#define RULE_H

#include "dialog.h"

namespace Ui
{
    class RuleDialog;
}

class DialogRule : public QDialog
{
    Q_OBJECT

public:
    explicit DialogRule(QWidget* parent = 0);
    DialogRule(const QString &, const QString &, QWidget* parent = 0);
    ~DialogRule();

    RuleSet result(void) const;

private slots:
    void on_lineEditFind_textChanged(const QString &);

private:
    bool checkButton(void);

    Ui::RuleDialog* ui;
};

#endif // RULE_H
