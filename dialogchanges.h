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

#ifndef DIALOGCHANGES_H
#define DIALOGCHANGES_H

#include <QDialog>

namespace Ui {
class DialogChanges;
}

class DialogChanges : public QDialog
{
    Q_OBJECT

public:
    explicit DialogChanges(const QString & ruleset, QWidget *parent = 0);
    ~DialogChanges();

    void    addContent(const QString &, const QString &, const QString &);

private:
    Ui::DialogChanges *ui;
};

#endif // DIALOGCHANGES_H
