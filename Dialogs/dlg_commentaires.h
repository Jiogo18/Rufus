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

#ifndef DLG_COMMENTAIRES_H
#define DLG_COMMENTAIRES_H

#include <QGraphicsOpacityEffect>
#include <QMenu>
#include <QToolTip>

#include "procedures.h"
#include "upcheckbox.h"
#include "uplabel.h"
#include "uplineedit.h"
#include "uptablewidget.h"
#include "widgetbuttonframe.h"

namespace Ui {
class dlg_commentaires;
}

class dlg_commentaires : public UpDialog
{
    Q_OBJECT

public:
    dlg_commentaires(Procedures *procAPasser, QWidget *parent = Q_NULLPTR);
    ~dlg_commentaires();
    Ui::dlg_commentaires *ui;

    QString                 gReponseResumePrescription, gReponseResumeDossier;

protected:
    void                    changeEvent(QEvent *e);

private:
    void                        ChoixMenuContextuel(QString);

    void                        New_Com();
    void                        Del_Com();
    void                        Modif_Com();
    bool                        gModifSurvol;
    bool                        gModeSurvol;
    double                      gOpacity;
    int                         gidUser;
    int                         gMode;
    enum gMode                  {Selection, Modification, Creation};
    WidgetButtonFrame           *widgButtons;

    QGraphicsOpacityEffect      *gOp;
    QSqlDatabase                db;
    QString                     textDocument, resumeDocument;
    QTimer                      *gTimerEfface;
    Procedures                  *proc;

    bool                        eventFilter(QObject *, QEvent *);
    void                        keyPressEvent   (QKeyEvent * event );
    bool                        ChercheDoublon(QString str, int row);
    void                        ConfigMode(int mode, int row = 0);
    void                        EffaceWidget(QWidget* widg, bool AvecOuSansPause = true);
    void                        DisableLines();
    void                        EnableLines();
    void                        InsertCommentaire(int row);
    void                        LineSelect(int row);
    void                        UpdateCommentaire(int row);
    void                        SupprimmCommentaire(int row);
    void                        Remplir_TableView();

    //anciens slots
    void                        Annulation();
    void                        ChoixButtonFrame(int);
    void                        ComCellEnter(int row);
    void                        MenuContextuel(UpLineEdit *line);
    void                        dblClicktextEdit();
    void                        EnableOKPushbutton();
    void                        Validation();
};

#endif // DLG_COMMENTAIRES_H
