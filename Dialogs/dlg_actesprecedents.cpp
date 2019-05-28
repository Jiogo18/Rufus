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

#include "dlg_actesprecedents.h"
#include "ui_dlg_actesprecedents.h"

//Uniquement consultative, cette fiche ne permet de modifier aucun élément de la base
dlg_actesprecedents::dlg_actesprecedents(Actes *actes, bool AvantDernier, QWidget *parent) :
    QDialog(parent),

ui(new Ui::dlg_actesprecedents)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint | Qt::WindowMinMaxButtonsHint);
    m_listeActes = actes;
    m_currentpatient = Datas::I()->patients->getById(m_listeActes->actes()->last()->idPatient());
    m_idpatient = m_listeActes->actes()->last()->idPatient();
    setWindowTitle(tr("Consultations précédentes de ") + m_currentpatient->nom() + " " + m_currentpatient->prenom());
    setWindowIcon(Icons::icLoupe());
    proc            = Procedures::I();
    gAvantDernier   = AvantDernier;
    setAttribute(Qt::WA_DeleteOnClose);
    setAttribute(Qt::WA_ShowWithoutActivating);
    QString style = "background-color:qlineargradient(x1: 0, y1: 0, x2: 1, y2: 1, stop: 0 #f6f7fa, stop: 1 rgba(200, 230, 250, 50));"
                    " border: 1px solid rgb(150,150,150); border-radius: 10px;";
    ui->EnteteupLabel           ->setStyleSheet(style);
    ui->CorpsupTextEdit         ->setStyleSheet(style);
    ui->ConclusionupTextEdit    ->setStyleSheet(style);
    ui->FermepushButton->installEventFilter(this);

    if (gAvantDernier)
        restoreGeometry(proc->gsettingsIni->value("PositionsFiches/PositionActesPrec").toByteArray());
    else
        restoreGeometry(proc->gsettingsIni->value("PositionsFiches/PositionAutreDossier").toByteArray());

    connect (ui->FermepushButton,           &QPushButton::clicked,  this,   &dlg_actesprecedents::close);
    connect (ui->ActeSuivantpushButton,     &QPushButton::clicked,  this,   [=] {NavigationConsult(Suiv);});
    connect (ui->DernierActepushButton,     &QPushButton::clicked,  this,   [=] {NavigationConsult(Fin);});
    connect (ui->PremierActepushButton,     &QPushButton::clicked,  this,   [=] {NavigationConsult(Debut);});
    connect (ui->ActePrecedentpushButton,   &QPushButton::clicked,  this,   [=] {NavigationConsult(Prec);});

    ui->FermepushButton->setShortcut(QKeySequence("Meta+Return"));
    proc->ModifTailleFont(ui->RenseignementsWidget, -3);

    Actualise();
}

dlg_actesprecedents::~dlg_actesprecedents()
{
    delete ui;
}

/*!
 * \brief dlg_actesprecedents::Actualise
 * Cette fonction est appelée par Rufus.cpp
 * quand un acte est créé ou supprimé
 */
void dlg_actesprecedents::Actualise()
{
    if( m_listeActes->actes()->size() == 0 )
    {
        //ERROR
        //tr("Impossible de retrouver la dernière consultation")
        return;
    }

    int initScrollValue;
    itCurrentActe = m_listeActes->getLast();
    initScrollValue = m_listeActes->actes()->size() - 1;
    if( gAvantDernier )
    {
        --initScrollValue;
        --itCurrentActe;
        if( itCurrentActe == Q_NULLPTR )
        {
            initScrollValue = 0;
            itCurrentActe = m_listeActes->actes()->constBegin();
        }
    }
    ui->ScrollBar->disconnect();
    ui->ScrollBar->setMinimum(0);
    ui->ScrollBar->setMaximum(m_listeActes->actes()->size() - 1);
    ui->ScrollBar->setSingleStep(1);
    ui->ScrollBar->setValue(initScrollValue);
    ActesPrecsAfficheActe();

    if( ui->ScrollBar->maximum() > 0 )
        connect(ui->ScrollBar, &QScrollBar::valueChanged, this, [=](int newValue) {
            itCurrentActe = m_listeActes->getAt(newValue);
            ActesPrecsAfficheActe();
        });
}

//Je n'arrives pas à le tester, les touches se sont pas capturées
//sur ma VM Linux
//J'ai toutes les touches sauf les fleches, page up et page down.
void dlg_actesprecedents::keyPressEvent(QKeyEvent *keyEvent)
{
    if (keyEvent->key() == Qt::Key_Down)
    {
        if (ui->ScrollBar->value() >= ui->ScrollBar->maximum())
            return;

        ++itCurrentActe;
        ActesPrecsAfficheActe();
    }

    if (keyEvent->key() == Qt::Key_Up)
    {
        if (ui->ScrollBar->value() <= ui->ScrollBar->minimum())
            return;

        --itCurrentActe;
        if( itCurrentActe == Q_NULLPTR)
            itCurrentActe = m_listeActes->actes()->constBegin();
        ActesPrecsAfficheActe();
    }


    if (keyEvent->key() == Qt::Key_PageDown)
    {
        /*if (ui->ScrollBar->value()+ui->ScrollBar->pageStep() < ui->ScrollBar->maximum())
            ActesPrecsAfficheActe(listactes.at(ui->ScrollBar->value()+ui->ScrollBar->pageStep()));
        else
            ActesPrecsAfficheActe(listactes.last());*/
    }

    if (keyEvent->key() == Qt::Key_PageUp)
    {
        /*if (ui->ScrollBar->value()-ui->ScrollBar->pageStep() > ui->ScrollBar->minimum())
            ActesPrecsAfficheActe(listactes.at(ui->ScrollBar->value()-ui->ScrollBar->pageStep()));
        else
            ActesPrecsAfficheActe(listactes.first());*/
    }

}

void dlg_actesprecedents::wheelEvent(QWheelEvent *event)
{
    /* la roulette de la souris fait défiler les actes.
     * On neutralise la fonction si le curseur est dans ui->CorpsupTextEdit ou ui->ConclusionupTextEdit
     * pour pouvoir utiliser à la souris les ascenseurs éventuels dans ces zones
    */
    if (!ui->CorpsupTextEdit->underMouse() && !ui->ConclusionupTextEdit->underMouse())
    {
        int deplacemtsouris = event->angleDelta().y();
        if( deplacemtsouris > 0 ) //on affiche l'acte précédent
            NavigationConsult(Prec);
        else //on affiche l'acte suivant
            NavigationConsult(Suiv);
    }
}

void dlg_actesprecedents::closeEvent(QCloseEvent *event)
{
    if (gAvantDernier)
        proc->gsettingsIni->setValue("PositionsFiches/PositionActesPrec", saveGeometry());
    else
        proc->gsettingsIni->setValue("PositionsFiches/PositionAutreDossier", saveGeometry());
    event->accept();
}

// ------------------------------------------------------------------------------------------
// Interception des évènements internes
// ------------------------------------------------------------------------------------------
bool dlg_actesprecedents::eventFilter(QObject *obj, QEvent *event)
{
    if (obj == ui->FermepushButton)
    {
        QPushButton* Button = static_cast<QPushButton*>(obj);
        if(event->type() == QEvent::MouseButtonPress)
            Button->setIcon(Icons::icFermeAppuye());
        if(event->type() == QEvent::MouseButtonRelease)
            Button->setIcon(Icons::icFerme());
        if(event->type() == QEvent::MouseMove)
        {
            QRect rect = QRect(Button->mapToGlobal(QPoint(0,0)),Button->size());
            QPoint pos = cursor().pos();
            if (rect.contains(pos))
                Button->setIcon(Icons::icFermeAppuye());
            else
                Button->setIcon(Icons::icFerme());
        }
        return false;
    }
    return dlg_actesprecedents::eventFilter(obj, event);
}

/*------------------------------------------------------------------------------------------------------------------------------------
-- Afficher les champs ---------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------*/
void dlg_actesprecedents::ActesPrecsAfficheActe(Acte *acte)
{
    itCurrentActe = m_listeActes->actes()->find(acte->id());
    if( itCurrentActe == m_listeActes->actes()->constEnd() )
        return;
    ActesPrecsAfficheActe();
}

void dlg_actesprecedents::ActesPrecsAfficheActe()
{
    Acte *acte = itCurrentActe.value();
    User * usr = Datas::I()->users->getById(acte->idUser());

    if( acte == Q_NULLPTR )    // Aucune consultation trouvee pour ces criteres
        return;

    ui->idPatientlineEdit->setText(QString::number(acte->idPatient()));

    //1.  Afficher les éléments de la tables Actes
    ui->EnteteupLabel->clear();
    ui->CorpsupTextEdit->clear();
    ui->ConclusionupTextEdit->clear();

    QString textHTML = "<p style = \"margin-top:0px; margin-bottom:10px;\">"
                      "<td width=\"130\"><font color = \"" COULEUR_TITRES "\" ><u><b>" + acte->date().toString(tr("d MMMM yyyy")) + "</b></u></font></td>"
                      "<td width=\"60\">" + Item::CalculAge(acte->agePatient(), acte->date())["toString"].toString() + "</td>"
                      "<td width=\"400\">" + usr->getPrenom() + " " + usr->getNom() + "</td></p>";
    ui->EnteteupLabel->setText(textHTML);
    if( acte->motif().size() || acte->texte().size() || acte->conclusion().size() )
    {
        textHTML = "";
        if( acte->motif().size() )
        {
            QString texte = acte->motif();
            Utils::convertHTML(texte);
            textHTML += "<p style = \"margin-top:0px; margin-bottom:0px;\"><td width=\"450\"><font color = \"" COULEUR_TITRES "\">" + tr("MOTIF") + "</font></td></p>";
            textHTML += "<p style = \"margin-top:0px; margin-bottom:10px;\"><td width=\"10\"></td><td width=\"450\">" + texte + "</td></p>";
        }

        if( acte->texte().size() )
        {
            QString texte = acte->texte();
            Utils::convertHTML(texte);
            textHTML += "<p style = \"margin-top:0px; margin-bottom:0px;\" ><td width=\"450\"><font color = \"" COULEUR_TITRES "\">" + tr("EXAMEN") + "</font></td></p>";
            textHTML += "<p style = \"margin-top:0px; margin-bottom:10px;\" ><td width=\"10\"></td><td width=\"450\">" + texte + "</td></p>";
        }
        ui->CorpsupTextEdit->setText(textHTML);
        textHTML = "";

        if( acte->conclusion().size() )
        {
            QString texte = acte->conclusion();
            if( acte->motif().size() || acte->texte().size() )
                textHTML += "<p style = \"margin-top:0px; margin-bottom:0px;\" ><td width=\"450\"><font color = \"" COULEUR_TITRES "\">" + tr("CONCLUSION") + "</font></td></p>";
            textHTML += "<p style = \"margin-top:0px; margin-bottom:10px;\" ><td width=\"10\"></td><td width=\"450\">" + texte + "</td></p>";
        }
        ui->ConclusionupTextEdit->setText(textHTML);
        textHTML = "";
    }

    ui->CourrierAFairecheckBox->setChecked(acte->courrierAFaire());
    ui->idActelineEdit->setText(QString::number(acte->id()));

    //2. retrouver le créateur de l'acte
    //idUser = ActesPrecsQuery.value(2).toInt();
    ui->CreateurConsultlineEdit->setText(tr("Créé par ") + Datas::I()->users->getById(acte->idCreatedBy())->getLogin() +
                                         tr(" pour ") + usr->getLogin()); //Avant idPatient

    //3. Mettre à jour le numéro d'acte
    if( m_listeActes->actes()->size() > 1 )
    {
        int scrolPos = m_listeActes->actes()->keys().indexOf(acte->id());
        ui->ScrollBar->setValue(scrolPos);
    }

    bool canprec = (m_listeActes->actes()->size() > 1 && m_listeActes->actes()->keys().indexOf(acte->id()) > 0);
    ui->ActePrecedentpushButton->setEnabled(canprec);

    bool cansui = (m_listeActes->actes()->size() > 1 && m_listeActes->actes()->keys().indexOf(acte->id()) < m_listeActes->actes()->size() - 1);
    ui->ActeSuivantpushButton->setEnabled(cansui);

    bool canfirst = (m_listeActes->actes()->size() > 1 && m_listeActes->actes()->keys().indexOf(acte->id()) > 0);
    ui->PremierActepushButton->setEnabled(canfirst);

    bool canlast = (m_listeActes->actes()->size() > 1 && m_listeActes->actes()->keys().indexOf(acte->id()) < m_listeActes->actes()->size() - 1);
    ui->DernierActepushButton->setEnabled(canlast);

    ui->NoActelabel->setText(QString::number(m_listeActes->actes()->keys().indexOf(acte->id()) + 1) + " / " + QString::number(m_listeActes->actes()->size()));

    //4. Afficher les renseignements comptables
    ui->ActeCotationlineEdit->setText(acte->cotation());
    if( acte->isPayeEnFranc() )
    {
        ui->ActeMontantLabel->setFixedHeight(30);
        ui->ActeMontantLabel->setText("Montant (€)\n(payé en F)");
    }
    else
    {
        ui->ActeMontantLabel->setFixedHeight(15);
        ui->ActeMontantLabel->setText("Montant (€)");
    }
    ui->ActeMontantlineEdit->setText( QLocale().toString(acte->montant(),'f',2) ) ;

    if (acte->paiementType().isEmpty() )
    {
        ui->Comptaframe->setVisible(false);
    }
    else
    {
        ui->Comptaframe->setVisible(true);
        ui->PaiementlineEdit->setVisible(true);

        //2. on recheche ensuite le type de paiement : espèces, chèque, tiers, cb, impayé, gratuit
        if (acte->paiementType() == "C") ui->PaiementlineEdit->setText(tr("chèque"));
        else if (acte->paiementType() == "E") ui->PaiementlineEdit->setText(tr("espèces"));
        else if (acte->paiementType() == "B") ui->PaiementlineEdit->setText(tr("carte de crédit"));
        else if (acte->paiementType() == "G") ui->PaiementlineEdit->setText(tr("Acte gratuit"));
        else if (acte->paiementType() == "I") ui->PaiementlineEdit->setText(tr("Impayé"));
        else if (acte->paiementType() == "T" && acte->paiementTiers() == "CB") ui->PaiementlineEdit->setText(tr("carte de crédit"));
        else if (acte->paiementType() == "T" && acte->paiementTiers() != "CB") ui->PaiementlineEdit->setText(acte->paiementTiers());

        // on calcule le montant payé pour l'acte
        if (acte->paiementType() != "G" || acte->paiementType() != "I")
        {
            double montant = DataBase::I()->getActePaye(acte->id());
            ui->PayelineEdit->setText(QLocale().toString(montant,'f',2));
        }


        if (acte->paiementType() == "B"
              ||(acte->paiementType() == "T" && acte->paiementTiers() == "CB")
              || acte->paiementType() == "C"
              || acte->paiementType() == "E")
        {
            ui->PaiementLabel->setVisible(true);
            ui->PaiementLabel->setText("Paiement:");
            ui->PayeLabel->setVisible(true);
            ui->PayelineEdit->setVisible(true);
            ui->Comptaframe->setFixedHeight(74);
            ui->PaiementlineEdit->setGeometry(79,25,91,18);
        }
        else if (acte->paiementType() == "G"
                 || acte->paiementType() == "I")
        {
            ui->PaiementLabel->setVisible(false);
            ui->PayeLabel->setVisible(false);
            ui->PayelineEdit->setVisible(false);
            ui->Comptaframe->setFixedHeight(50);
            ui->PaiementlineEdit->setGeometry(8,25,164,18);
        }
        if (acte->paiementType() == "T" && acte->paiementTiers() != "CB")
        {
            ui->PaiementLabel->setText("Tiers");
            ui->PaiementLabel->setVisible(true);
            ui->PaiementlineEdit->setVisible(true);
            ui->PayeLabel->setVisible(true);
            ui->PayelineEdit->setVisible(true);
            ui->Comptaframe->setFixedHeight(74);
            ui->PaiementlineEdit->setGeometry(79,25,91,18);
        }
    }
}


/*------------------------------------------------------------------------------------------------------------------------------------
-- Retrouver l'acte à afficher -------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------*/
Acte* dlg_actesprecedents::getActeAffiche()
{
    return itCurrentActe.value();;
}

/*------------------------------------------------------------------------------------------------------------------------------------
-- Afficher la consultation précédente ou suivante d'un patient ----------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------------------*/
bool dlg_actesprecedents::NavigationConsult(int i)
{
    if( m_listeActes->actes()->size() == 1 )
        return true;

    int idActe = -1;
    if (i == Suiv)
    {
        ++itCurrentActe;
        if( itCurrentActe == m_listeActes->actes()->constEnd() )
            itCurrentActe = m_listeActes->getLast();
    }
    else if (i == Prec)
    {
        --itCurrentActe;
        if( itCurrentActe == Q_NULLPTR )
            itCurrentActe = m_listeActes->actes()->constBegin();
    }
    else if (i == Debut)
    {
        itCurrentActe = m_listeActes->actes()->constBegin();
    }
    else if (i == Fin)
    {
        itCurrentActe = m_listeActes->getLast();
    }

    idActe = itCurrentActe.value()->id();
    if (idActe > -1)
    {
        ActesPrecsAfficheActe();
        return true;
    }

    return false;
}

int dlg_actesprecedents::getidPatient()
{
    return m_idpatient;
}

