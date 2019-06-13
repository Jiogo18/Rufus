/* (C) 2018 LAINE SERGE
This file is part of RufusAdmin or Rufus.

RufusAdmin and Rufus are free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License,
or any later version.

RufusAdmin and Rufus are distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RufusAdmin and Rufus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef UPLINEEDIT_H
#define UPLINEEDIT_H

#include <QLineEdit>
#include <QDoubleValidator>
#include <QEvent>
#include <QSound>
#include <QToolTip>

class UpLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit    UpLineEdit(QWidget *parent = Q_NULLPTR);
    ~UpLineEdit();
    void        setRowTable(int val);
    int         getRowTable() const;
    void        setId(int id);
    int         getId();
    int         getColumnTable() const;
    void        setColumnTable(int val);
    void        setData(QVariant data);
    QVariant    getData();
    void        setValeurAvant(QString valprec);
    QString     getValeurAvant() const;
    void        setValeurApres(QString valpost);
    QString     getValeurApres() const;
    void        setChamp(QString champcorrespondant);
    QString     getChampCorrespondant() const;
    void        setTable(QString tablecorrespondant);
    QString     getTableCorrespondant() const;
    void        setImmediateToolTip(QString Msg);

private:
    QString     gToolTipMsg;
    bool        eventFilter(QObject *obj, QEvent *event)  ;
    int         RowTable, ColumnTable, id;
    QString     ValeurAvant, ValeurApres, Champ, Table;
    QVariant    linedata;
    void        AfficheToolTip();

signals:
    void        TextModified(QString);
    void        mouseEnter(int row);
    void        mouseRelease(int row);
    void        mouseDoubleClick(int row);

protected:
    void        enterEvent(QEvent *e);
    void        mouseReleaseEvent(QMouseEvent *e);
    void        mouseDoubleClickEvent(QMouseEvent *e);

};

#endif // UPLINEEDIT_H
