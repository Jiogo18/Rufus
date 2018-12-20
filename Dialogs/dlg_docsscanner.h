/* (C) 2016 LAINE SERGE
This file is part of Rufus.

Rufus is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Rufus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rufus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DLG_DOCSSCANNER_H
#define DLG_DOCSSCANNER_H

#include "updialog.h"
#include "poppler-qt5.h"
#include "procedures.h"
#include "upcombobox.h"
#include "uplabel.h"
#include "uplineedit.h"
#include "uptablewidget.h"
#include "uptoolbar.h"
#include "dlg_message.h"

class dlg_docsscanner : public UpDialog
{
    Q_OBJECT
public:
    explicit dlg_docsscanner(int idPatouFact, int mode = dlg_docsscanner::Document, QString titre = "", QWidget *parent = Q_NULLPTR);
    ~dlg_docsscanner();
    bool                    getinitOK();
    void                    NavigueVers(QString);
    QMap<QString, QVariant> getdataFacture();
    int                     gMode;
        enum gMode          {Document, Facture, Echeancier};

private:
    Procedures          *proc;
    DataBase            *db;
    int                 iditem;
    QMap<QString, QVariant> datafacture;
    UpLabel             *inflabel;
    UpLineEdit          *linetitre;
    QDateEdit           *editdate;
    UpComboBox          *typeDocCombo;
    UpTableWidget       *uptable;
    UpToolBar           *toolbar;
    UpPushButton        *dirsearchbutton;
    bool                initOK;
    QList<QPixmap>      glistPix;
    QStringList         ListTypeExams;
    QString             docpath;
    QString             NomDirStockageImagerie;
    QString             fichierimageencours;
    bool                eventFilter(QObject *, QEvent *);
    void                ValideFiche();
    void                ChangeFile();
    bool                AccesDistant;
};

#endif // DLG_DOCSSCANNER_H
