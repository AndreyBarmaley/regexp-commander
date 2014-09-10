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

#include <QDir>
#include <QPair>
#include <QFile>
#include <QMenu>
#include <QDebug>
#include <QRegExp>
#include <QSettings>
#include <QMessageBox>
#include <QTextStream>
#include <QFileInfo>
#include <QFileDialog>
#include <QStringList>

#include "dialog.h"
#include "dialogrule.h"
#include "dialogchanges.h"
#include "ui_dialog.h"

const QString backup_suffix("~");

struct Content : QStringList
{
    int changes;

    Content() {}

    Content applyRuleSet(const RuleSet & ruleset) const
    {
        Content replacedContent(*this);
	replacedContent.changes = 0;
        for(QStringList::Iterator
            it = replacedContent.begin(); it != replacedContent.end(); ++it)
        {
            int pos = -1;
            if(-1 != (pos = ruleset.findRegexp().indexIn(*it, 0)))
            {
                (*it).replace(ruleset.findRegexp(), ruleset.replacedString());
                replacedContent.changes++;
            }
        }
        return replacedContent;
    }
};

struct ContentRevision : QList<Content>
{
    ContentRevision() {}

    ContentRevision(const QString & fn)
    {
        QFile file(fn);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream ts(& file);
            QString line;
            Content content;
            while(true)
            {
                QString line = ts.readLine();
                if(line.isNull())
                    break;
                else
                    content << line;
            }
            append(content);
        }
    }

    void mergeRevisions()
    {
	if(size())
	{
	    Content latest = back();
	    clear();
	    append(latest);
	}
    }
};

struct FileRevision : public QTreeWidgetItem
{
    ContentRevision contentRevisions;

    FileRevision(const QString & fn)
    {
        setText(0, QFileInfo(fn).fileName());
        setData(0, Qt::ToolTipRole, fn);
        setData(0, Qt::UserRole, fn);
        setFlags(flags() | Qt::ItemIsUserCheckable);
        setCheckState(0, Qt::Checked);
        contentRevisions << ContentRevision(fn);
        if(contentRevisions.front().size() < contentRevisions.back().size())
            setText(1, QString("+").append(QString::number(contentRevisions.back().size() - contentRevisions.front().size())));
        else if(contentRevisions.front().size() > contentRevisions.back().size())
            setText(1, QString("-").append(QString::number(contentRevisions.front().size() - contentRevisions.back().size())));
    }

    int applyRuleSet(const RuleSet & ruleset, int revison)
    {
        while(contentRevisions.size() > revison + 1)
            contentRevisions.removeLast();
        Content newContent = contentRevisions.back().applyRuleSet(ruleset);
        contentRevisions << newContent;
        return newContent.changes;
    }
};

Dialog::Dialog(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::Dialog)
{
    ui->setupUi(this);
    actionAddRule = new QAction(tr("Add"), ui->treeWidgetRules);
    actionEditRule = new QAction(tr("Edit"), ui->treeWidgetRules);
    actionDeleteRule = new QAction(tr("Delete"), ui->treeWidgetRules);
    actionAddRule->setEnabled(true);
    actionEditRule->setEnabled(false);
    actionDeleteRule->setEnabled(false);
    connect(ui->pushButtonSelect, SIGNAL(clicked()), this, SLOT(selectPath()));
    connect(actionAddRule, SIGNAL(triggered()), this, SLOT(addNewRuleset()));
    connect(actionEditRule, SIGNAL(triggered()), this, SLOT(editCurrentRuleset()));
    connect(actionDeleteRule, SIGNAL(triggered()), this, SLOT(deleteCurrentRuleset()));
    connect(this, SIGNAL(rulesetChanged(int)), this, SLOT(applyRuleset(int)));
    connect(ui->pushButtonAction, SIGNAL(clicked()), this, SLOT(applyChanges()));
    connect(ui->pushButtonClose, SIGNAL(clicked()), this, SLOT(close()));
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    setGeometry(qvariant_cast<QRect>(settings.value("mainDialogSize", geometry())));
    ui->treeWidgetRules->setColumnWidth(0, qvariant_cast<int>(settings.value("rulesColumnWidth0", 250)));
    ui->treeWidgetRules->setColumnWidth(1, qvariant_cast<int>(settings.value("rulesColumnWidth1", 250)));
    ui->treeWidgetRules->setColumnWidth(2, qvariant_cast<int>(settings.value("rulesColumnWidth2", 20)));
    ui->treeWidgetFiles->setColumnWidth(0, qvariant_cast<int>(settings.value("filesColumnWidth0", 400)));
    ui->treeWidgetFiles->setColumnWidth(1, qvariant_cast<int>(settings.value("filesColumnWidth1", 100)));
}

Dialog::~Dialog()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    settings.setValue("mainDialogSize", geometry());
    settings.setValue("rulesColumnWidth0", ui->treeWidgetRules->columnWidth(0));
    settings.setValue("rulesColumnWidth1", ui->treeWidgetRules->columnWidth(1));
    settings.setValue("rulesColumnWidth2", ui->treeWidgetRules->columnWidth(2));
    settings.setValue("filesColumnWidth0", ui->treeWidgetFiles->columnWidth(0));
    settings.setValue("filesColumnWidth1", ui->treeWidgetFiles->columnWidth(1));
    delete ui;
}

void Dialog::selectPath()
{
    QSettings settings(QSettings::IniFormat, QSettings::UserScope, "QtRegexpCommander", "settings");
    QString latestPath = qvariant_cast<QString>(settings.value("latestPath", ""));
    QString dirPath = QFileDialog::getExistingDirectory(this, tr("Open Directory"), latestPath.isEmpty() ? QDir::homePath() : latestPath, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dirPath.size())
    {
        settings.setValue("latestPath", dirPath);
        QStringList listFiles = QDir(dirPath).entryList(QDir::Files | QDir::Readable);
        for(QStringList::ConstIterator
            it = listFiles.begin(); it != listFiles.end(); ++it)
            if(!(*it).contains(QRegExp(QString(backup_suffix).append("$"))))
                ui->treeWidgetFiles->addTopLevelItem(new FileRevision(QDir::toNativeSeparators(QString(dirPath).append("/").append(*it))));
        ui->lineEditPath->setText(dirPath);
    }
}

void Dialog::on_treeWidgetRules_customContextMenuRequested(const QPoint & pt)
{
    QMenu menu(ui->treeWidgetRules);
    actionEditRule->setEnabled(ui->treeWidgetRules->selectedItems().size());
    actionDeleteRule->setEnabled(ui->treeWidgetRules->selectedItems().size());
    menu.addAction(actionAddRule);
    menu.addAction(actionEditRule);
    menu.addSeparator();
    menu.addAction(actionDeleteRule);
    menu.exec(pos() + pt + QPoint(30, 60));
}

void Dialog::addNewRuleset()
{
    DialogRule dialog;
    if(QDialog::Accepted == dialog.exec())
    {
        RuleSet ruleset = dialog.result();
        QTreeWidgetItem* item = new QTreeWidgetItem(QStringList() << ruleset.findRegexp().pattern() << ruleset.replacedString() << QString::number(0));
        item->setData(0, Qt::UserRole, ruleset.regexpPattern());
        item->setData(1, Qt::UserRole, ruleset.replacedString());
        item->setData(2, Qt::UserRole, 0);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable);
        item->setCheckState(0, Qt::Checked);
        ui->treeWidgetRules->addTopLevelItem(item);
        emit rulesetChanged(ui->treeWidgetRules->topLevelItemCount() - 1);
    }
}

void Dialog::editCurrentRuleset()
{
    DialogRule dialog(ui->treeWidgetRules->currentItem()->text(0), ui->treeWidgetRules->currentItem()->text(1));
    if(QDialog::Accepted == dialog.exec())
    {
        RuleSet ruleset = dialog.result();
        QTreeWidgetItem* item = ui->treeWidgetRules->currentItem();
        item->setText(0, ruleset.regexpPattern());
        item->setText(1, ruleset.replacedString());
        item->setData(0, Qt::UserRole, ruleset.regexpPattern());
        item->setData(1, Qt::UserRole, ruleset.replacedString());
        item->setText(2, QString::number(0));
        emit rulesetChanged(ui->treeWidgetRules->currentIndex().row());
    }
}

void Dialog::deleteCurrentRuleset()
{
    int ruleIndex = ui->treeWidgetRules->currentIndex().row();
    ui->treeWidgetRules->takeTopLevelItem(ruleIndex);
    emit rulesetChanged(ruleIndex);
}

void Dialog::on_treeWidgetRules_doubleClicked(const QModelIndex & index)
{
    if(index.column() == 0 || index.column() == 1)
        editCurrentRuleset();
    else if(index.column() == 2)
    {
        int count = ui->treeWidgetRules->currentItem()->text(2).toInt();
        if(count)
        {
            DialogChanges dialog(ui->treeWidgetRules->currentItem()->text(0), this);
            for(int indexFile = 0; indexFile < ui->treeWidgetFiles->topLevelItemCount(); ++indexFile)
            {
                FileRevision* fr = dynamic_cast<FileRevision*>(ui->treeWidgetFiles->topLevelItem(indexFile));
                if(fr->contentRevisions.size() <= index.row() + 1)
                    QMessageBox::warning(this, "Sorry", QString("The revision not found: %d").arg(index.row()));
                else
                {
                    Content content1 = fr->contentRevisions[index.row()];
                    Content content2 = fr->contentRevisions[index.row() + 1];
                    if(content1.size() != content2.size())
                        QMessageBox::warning(this, "Sorry", QString("The contents not match size: %d vs %d").arg(index.row()).arg(index.row() + 1));
                    else
                    {
                        for(int pos = 0; pos < content1.size(); ++pos)
                        {
                            if(content1[pos] != content2[pos])
                                dialog.addContent(qvariant_cast<QString>(fr->data(0, Qt::UserRole)), content1[pos], content2[pos]);
                        }
                    }
                }
            }
            dialog.exec();
        }
    }
}

void Dialog::applyRuleset(int index)
{
    for(int indexRule = index; indexRule < ui->treeWidgetRules->topLevelItemCount(); ++indexRule)
    {
        QTreeWidgetItem* itemRule = ui->treeWidgetRules->topLevelItem(indexRule);
        if(Qt::Unchecked == itemRule->checkState(0)) continue;
        const QString & data0 = qvariant_cast<QString>(itemRule->data(0, Qt::UserRole));
        const QString & data1 = qvariant_cast<QString>(itemRule->data(1, Qt::UserRole));
        RuleSet ruleset(QRegExp(data0), data1);
	int total = 0;
        for(int indexFile = 0; indexFile < ui->treeWidgetFiles->topLevelItemCount(); ++indexFile)
        {
            FileRevision* fr = dynamic_cast<FileRevision*>(ui->treeWidgetFiles->topLevelItem(indexFile));
            if(Qt::Unchecked == fr->checkState(0)) continue;
            total += fr ? fr->applyRuleSet(ruleset, indexRule) : 0;
        }
        itemRule->setText(2, QString::number(total));
    }
    ui->pushButtonAction->setEnabled(false);
    for(int indexRule = 0; indexRule < ui->treeWidgetRules->topLevelItemCount(); ++indexRule)
    {
        QTreeWidgetItem* itemRule = ui->treeWidgetRules->topLevelItem(indexRule);
        int count = itemRule->text(2).toInt();
        if(count)
        {
            ui->pushButtonAction->setEnabled(true);
            break;
        }
    }
}

void Dialog::applyChanges()
{
    for(int indexFile = 0; indexFile < ui->treeWidgetFiles->topLevelItemCount(); ++indexFile)
    {
        FileRevision* fr = dynamic_cast<FileRevision*>(ui->treeWidgetFiles->topLevelItem(indexFile));
        if(Qt::Unchecked == fr->checkState(0)) continue;
        if(fr->contentRevisions.size() < 2) continue;
        bool nochanges = true;
        for(ContentRevision::ConstIterator
            it = fr->contentRevisions.begin(); it != fr->contentRevisions.end(); ++it)
            if((*it).changes > 0)
            {
                nochanges = false;
                break;
            }
        if(nochanges) continue;

        QString origName = qvariant_cast<QString>(fr->data(0, Qt::UserRole));
        QString backName = QString(origName).append(backup_suffix);
        QFile(origName).rename(backName);

        QFile file(origName);
        if(file.open(QIODevice::WriteOnly))
        {
            QTextStream ts(& file);
            for(Content::ConstIterator
                it = fr->contentRevisions.back().begin(); it != fr->contentRevisions.back().end(); ++it)
                ts << *it << "\n";
        }
	fr->contentRevisions.mergeRevisions();
    }
    for(int indexRule = 0; indexRule < ui->treeWidgetRules->topLevelItemCount(); ++indexRule)
    {
        QTreeWidgetItem* itemRule = ui->treeWidgetRules->topLevelItem(indexRule);
        itemRule->setCheckState(0, Qt::Unchecked);
        itemRule->setText(2, QString::number(0));
    }
    ui->pushButtonAction->setEnabled(false);
}

void Dialog::on_treeWidgetFiles_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 0)
    {
        if(Qt::Checked == item->checkState(0))
            qDebug() << "checked file: " << qvariant_cast<QString>(item->data(0, Qt::UserRole));
        else
        if(Qt::Unchecked == item->checkState(0))
            qDebug() << "unchecked file: " << qvariant_cast<QString>(item->data(0, Qt::UserRole));
    }
}

void Dialog::on_treeWidgetRules_itemClicked(QTreeWidgetItem *item, int column)
{
    if(column == 0)
    {
        if(Qt::Checked == item->checkState(0))
            qDebug() << "checked rule";
        else
        if(Qt::Unchecked == item->checkState(0))
            qDebug() << "unchecked rule";
    }
}
