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

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QModelIndex>
#include <QPair>
#include <QRegExp>

namespace Ui
{
    class Dialog;
    class Rule;
}

class QTreeWidgetItem;

struct RuleSet : protected QPair<QRegExp, QString>
{
    RuleSet(const QRegExp & find, const QString & replace) : QPair<QRegExp, QString>(find, replace) {}

    QString regexpPattern(void) const
    {
        return first.pattern();
    }
    const QRegExp & findRegexp(void) const
    {
        return first;
    }
    const QString & replacedString(void) const
    {
        return second;
    }
};

class Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Dialog(QWidget* parent = 0);
    ~Dialog();

signals:
    void    rulesetChanged(int);

private slots:
    void    selectPath();
    void    addNewRuleset();
    void    editCurrentRuleset();
    void    deleteCurrentRuleset();
    void    applyRuleset(int);
    void    applyChanges();
    void    on_treeWidgetRules_customContextMenuRequested(const QPoint &);
    void    on_treeWidgetRules_doubleClicked(const QModelIndex &);

    void on_treeWidgetFiles_itemClicked(QTreeWidgetItem *item, int column);

    void on_treeWidgetRules_itemClicked(QTreeWidgetItem *item, int column);

private:
    Ui::Dialog* ui;
    Ui::Rule*   rule;
    QAction*    actionAddRule;
    QAction*    actionEditRule;
    QAction*    actionDeleteRule;
};

#endif // DIALOG_H
