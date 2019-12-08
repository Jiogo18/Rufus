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

#include "gbl_datas.h"
#include "dlg_remisecheques.h"
#include "icons.h"
#include "ui_dlg_remisecheques.h"
#include <QDebug>

dlg_remisecheques::dlg_remisecheques(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlg_remisecheques)
{

    //TODO on ne peut pas afficher les remises d'un utilisateur qui est désactivé ou qui n'a pas de chèque en attente

    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    restoreGeometry(proc->settings()->value("PositionsFiches/PositionRemiseCheques").toByteArray());

    connect (ui->AnnulupPushButton,                         &QPushButton::clicked,                  this,   &dlg_remisecheques::AnnulupPushButton);
    connect (ui->ListeChequesupTableWidget,                 &QTableWidget::cellEntered,             this,   &dlg_remisecheques::ToolTip);
    connect (ui->ChequesEnAttenteupTableWidget,             &QTableWidget::cellEntered,             this,   &dlg_remisecheques::ToolTip);
    connect (ui->ImprimeupPushButton,                       &QPushButton::clicked,                  this,   &dlg_remisecheques::ImprimepushButton);

    ui->ImprimeupPushButton->move(this->size().width()-123,this->size().height()-60);
    ui->AnnulupPushButton->move(this->size().width()-233,this->size().height()-60);
    ui->RemisesPrecsPushButton->move(7,this->size().height()-60);

    ui->ListeChequesupTableWidget->installEventFilter(this);


    ui->ListeChequesupTableWidget->setColumnCount(7);
    ui->ChequesEnAttenteupTableWidget->setColumnCount(7);
    ui->ListeChequesupTableWidget->setColumnWidth(0,30);     //Checkbox
    ui->ListeChequesupTableWidget->setColumnWidth(1,30);     //No Ligne
    ui->ListeChequesupTableWidget->setColumnWidth(2,190);    //Tireur
    ui->ListeChequesupTableWidget->setColumnWidth(3,80);     //Banque
    ui->ListeChequesupTableWidget->setColumnWidth(4,58);     //Montant
    ui->ListeChequesupTableWidget->setColumnWidth(5,50);     //idCheque
    ui->ListeChequesupTableWidget->setColumnWidth(6,0);      //idrecspec
    ui->ListeChequesupTableWidget->setColumnHidden(5,true);
    ui->ListeChequesupTableWidget->setColumnHidden(6,true);

    ui->ChequesEnAttenteupTableWidget->setColumnWidth(0,30);    //Checkbox
    ui->ChequesEnAttenteupTableWidget->setColumnWidth(1,30);    //No Ligne
    ui->ChequesEnAttenteupTableWidget->setColumnWidth(2,190);   //Tireur
    ui->ChequesEnAttenteupTableWidget->setColumnWidth(3,80);    //Banque
    ui->ChequesEnAttenteupTableWidget->setColumnWidth(4,58);    //Montant
    ui->ChequesEnAttenteupTableWidget->setColumnWidth(5,50);    //idCheque
    ui->ChequesEnAttenteupTableWidget->setColumnWidth(6,0);     //idrecspec
    ui->ChequesEnAttenteupTableWidget->setColumnHidden(5,true);
    ui->ChequesEnAttenteupTableWidget->setColumnHidden(6,true);

    ui->ListeChequesupTableWidget->setGridStyle(Qt::DashDotDotLine);
    ui->ChequesEnAttenteupTableWidget->setGridStyle(Qt::DashDotDotLine);

    QStringList         LabelTableCheques;
    LabelTableCheques << " ";
    LabelTableCheques << "N°";
    LabelTableCheques << "Tireur";
    LabelTableCheques << "Banque";
    LabelTableCheques << "Montant";
    LabelTableCheques << "";
    LabelTableCheques << "";
    ui->ListeChequesupTableWidget->setHorizontalHeaderLabels(LabelTableCheques);
    ui->ChequesEnAttenteupTableWidget->horizontalHeader()->setVisible(false);

    UpLineEdit *ListRem = new UpLineEdit(this);
    ListRem->setStyleSheet(
    "UpLineEdit {background-color:white; border-style: none;}"
    "UpLineEdit:focus {border-style:none;}");
    ListRem->setReadOnly(true);
    ui->RemisePrecsupComboBox->setLineEdit(ListRem);

    ReconstruitListeUsers();
    if (!m_initok)
        return;
    connect (ui->UserComboBox,          QOverload<int>::of(&QComboBox::currentIndexChanged),    this,   &dlg_remisecheques::ChangeUser);
    connect (ui->ComptecomboBox,        QOverload<int>::of(&QComboBox::currentIndexChanged),    this,   &dlg_remisecheques::ChangeCompte);
    VoirNouvelleRemise();
    m_initok = true;
}

dlg_remisecheques::~dlg_remisecheques()
{
}

void dlg_remisecheques::reject()
{
    if (ui->ListeChequesupTableWidget->currentColumn() == 4)
    {
        QLineEdit* Line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(ui->ListeChequesupTableWidget->currentRow(),4));
        QString b ;
        b.setNum(Line->text().toDouble(),'f',2);
        Line->setText(m_valeuravantchangement);
        return;
    }
    QDialog::reject();
}


void dlg_remisecheques::AnnulupPushButton()
{
    if (m_mode == NouvelleRemise) {
        ui->ListeChequesupTableWidget->setCurrentCell(0,0); // pour valider une éventuelle modification dans un champ montant
        reject();
    }
    else
        VoirNouvelleRemise();
}

void dlg_remisecheques::CorrigeRemise()
{
    if (UpMessageBox::Question(this, tr("Suppression demandée"), tr("Êtes vous sûr de vouloir supprimer cette remise de chèques?")) != UpSmallButton::STARTBUTTON)
        return;
    QMap<QString, QVariant> MapRemise =  ui->RemisePrecsupComboBox->currentData().toMap();
    int idRemise = MapRemise["idRemise"].toInt();
    bool a = true;
    while (a) {
        a = db->createtransaction(QStringList()    << TBL_REMISECHEQUES
                                                << TBL_RECETTESSPECIALES
                                                << TBL_RECETTES
                                                << TBL_LIGNESCOMPTES);
        if (!a) break;
        // supprimer la remise dans la table Remises
        a = db->SupprRecordFromTable(idRemise, "idRemcheq", TBL_REMISECHEQUES);
        if (!a) break;
        // supprimer l'idRemise dans les lignes de la table lignesrecettes
        a = db->StandardSQL("update " TBL_RECETTES " set idremise = null, DateEnregistrement = null where idremise = " + QString::number(idRemise));
        if (!a) break;
        // supprimer l'idRemise dans les lignes de la table autresrecettes
        a = db->StandardSQL("update " TBL_RECETTESSPECIALES " set idremise = null, DateEnregistrement = null where idremise = " + QString::number(idRemise));
        if (!a) break;
        // supprimer la remise dans la table lignescomptes
        a = db->SupprRecordFromTable(idRemise, "idremcheq", TBL_LIGNESCOMPTES);
        break;
    }
    if (!a)
    {
        db->rollback();
        UpMessageBox::Watch(this,tr("Suppression impossible"));
    }
    else db->commit();

    // revenir au mode nouvelle remise
    VoirNouvelleRemise();
}

void dlg_remisecheques::ImprimepushButton()
{
    bool ok;
    if (ui->ListeChequesupTableWidget->rowCount() == 0)
    {
        UpMessageBox msgbox;
        msgbox.setText(tr("Il n'y a rien à imprimer!"));
        msgbox.setIcon(UpMessageBox::Warning);
        UpSmallButton OKBouton(tr("OK je corrige"));
        msgbox.addButton(&OKBouton, UpSmallButton::STARTBUTTON);
        msgbox.exec();
        if (msgbox.clickedButton() != &OKBouton) reject();
        return;
    }
    QString A;
    QString req;
    ui->ListeChequesupTableWidget->setCurrentCell(0,0); // pour valider une éventuelle modification dans un champ montant

    if (m_mode == NouvelleRemise) {
        // On vérifie que toutes les banques sont identifiées
        for (int i = 0; i < ui->ListeChequesupTableWidget->rowCount(); i++)
        {
            A = ui->ListeChequesupTableWidget->item(i,3)->text();
            if (A == "")
            {
                ui->ListeChequesupTableWidget->setCurrentCell(i,3);
                UpMessageBox::Watch(this,tr("Vous avez oublié d'identifier la banque émettrice de ce chèque!"));
                return;
            }
        }
        // On vérifie que tous les tireurs sont identifiés
        for (int i = 0; i < ui->ListeChequesupTableWidget->rowCount(); i++)
        {
            A = ui->ListeChequesupTableWidget->item(i,2)->text();
            if (A == "")
            {
                ui->ListeChequesupTableWidget->setCurrentCell(i,2);
                UpMessageBox::Watch(this,tr("Vous avez oublié d'identifier le tireur de ce chèque!"));
                return;
            }
        }
        if (ui->UserComboBox->currentText()=="")
        {
            ui->UserComboBox->setFocus();
            UpMessageBox::Watch(this,tr("Vous avez oublié d'identifier l'utilisateur qui encaissera les chèques!"));
            return;
        }
        if (ui->ComptecomboBox->currentText()=="")
        {
            ui->ComptecomboBox->setFocus();
            UpMessageBox::Watch(this,tr("Vous avez oublié d'identifier le compte d'encaissement des chèques!"));
            return;
        }

        // Recalcul du total
        double Total = 0;
        for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
        {
            QLineEdit* Line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(k,4));
            Total = Total + QLocale().toDouble(Line->text());
        }

        // On verrouille les table RemisesCheques
        if (!db->createtransaction(QStringList()    << TBL_REMISECHEQUES
                                                    << TBL_RECETTESSPECIALES
                                                    << TBL_RECETTES
                                                    << TBL_LIGNESCOMPTES
                                                    << TBL_ARCHIVESBANQUE))
            return;
        //On récupére l'idRemise
        int idRemise = db->selectMaxFromTable("idRemCheq", TBL_REMISECHEQUES, ok) + 1;
        // on enregistre la remise dans la table remisecheques
        if (!db->StandardSQL(" INSERT INTO " TBL_REMISECHEQUES " (idRemCheq, Montant, RCDate, idCompte) VALUES (" + QString::number(idRemise) +
                                "," + QString::number(Total) + ", NOW(),'" + ui->ComptecomboBox->currentData().toString() +"')"))
        {
            db->rollback();
            return;
        }

        // On imprime la remise
        if (!ImprimerRemise(idRemise))
        {
            db->rollback();
            UpMessageBox::Watch(this,tr("Impression annulée"));
            return;
        }

        // on enregistre les lignes de remises
        //  et on update tous les chèques déposés avec cet IdRemise dans la table chèques
        for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
        {
            QString RecSpec = ui->ListeChequesupTableWidget->item(k,6)->text();
            int idChequeAMettreAJour = ui->ListeChequesupTableWidget->item(k,5)->text().toInt();
            if (RecSpec=="1")
                req = "UPDATE " TBL_RECETTESSPECIALES " SET idRemise = " + QString::number(idRemise) + ", EnAttente = null WHERE idRecette = " + QString::number(idChequeAMettreAJour);
            else
                req = "UPDATE " TBL_RECETTES " SET idRemise = " + QString::number(idRemise) + ", EnAttente = null WHERE idRecette = " + QString::number(idChequeAMettreAJour);
            if (!db->StandardSQL(req))
            {
                db->rollback();
                UpMessageBox::Watch(this,tr("Impression annulée"));
                return;
            }
        }

        // On enregitre dans la table GestionComptes cettte remise
        req =  "INSERT INTO " TBL_LIGNESCOMPTES " (idLigne, idCompte,LigneDate,LigneLibelle,LigneMontant,LigneDebitCredit,LigneTypeOperation, idremcheq) VALUES (" +
                QString::number(db->getIdMaxTableComptesTableArchives()) + "," +
                ui->ComptecomboBox->currentData().toString() +
                ", NOW(),"
                "'" + tr("Remise de chèques n°") + QString::number(idRemise) +
                "','" + QString::number(Total) +
                "',1,'" + tr("Remise de chèques") + "', " + QString::number(idRemise) + ")";
        if (!db->StandardSQL(req))
        {
            db->rollback();
            UpMessageBox::Watch(this,tr("Impression annulée"));
            return;
        }

        // On corrige les intitulés de banque et les tireurs dans ActeBanque de la table Actes
        for (int l = 0; l < ui->ListeChequesupTableWidget->rowCount(); l++)
        {
            QString RecSpec = ui->ListeChequesupTableWidget->item(l,6)->text();
            QString AB = ui->ListeChequesupTableWidget->item(l,3)->text();
            if (RecSpec=="1")
                req = "UPDATE " TBL_RECETTESSPECIALES " SET BanqueCheque = '" + AB  + "', TireurCheque = '" + Utils::correctquoteSQL(ui->ListeChequesupTableWidget->item(l,2)->text()) +
                    + "', DateEnregistrement = NOW() WHERE idRecette = " + ui->ListeChequesupTableWidget->item(l,5)->text();
            else
                req = "UPDATE " TBL_RECETTES " SET BanqueCheque = '" + AB  + "', TireurCheque = '" + Utils::correctquoteSQL(ui->ListeChequesupTableWidget->item(l,2)->text()) +
                        + "', DateEnregistrement = NOW() WHERE idRecette = " + ui->ListeChequesupTableWidget->item(l,5)->text();
            if (!db->StandardSQL(req))
            {
                db->rollback();
                UpMessageBox::Watch(this,tr("Impression annulée"));
                return;
            }
        }

        // On corrige les chèques mis en attente
        QString chqatt, chqattrecspec;
        for (int l = 0; l < ui->ChequesEnAttenteupTableWidget->rowCount(); l++)
        {
            QString RecSpec = ui->ChequesEnAttenteupTableWidget->item(l,6)->text();
            if (RecSpec=="1")
                chqatt += ui->ChequesEnAttenteupTableWidget->item(l,5)->text() + ",";
            else
                chqattrecspec += ui->ChequesEnAttenteupTableWidget->item(l,5)->text() + ",";
        }
        if (chqatt!= "")
        {
            chqatt = chqatt.left(chqatt.size()-1);
            req = "UPDATE " TBL_RECETTES " SET EnAttente = 1 WHERE idRecette in (" + chqatt + ")";
            if (!db->StandardSQL(req))
            {
                db->rollback();
                UpMessageBox::Watch(this,tr("Impression annulée"));
                return;
            }
        }
        if (chqatt!= "")
        {
            chqattrecspec = chqattrecspec.left(chqattrecspec.size()-1);
            req = "UPDATE " TBL_RECETTESSPECIALES " SET EnAttente = 1 WHERE idRecette in (" + chqattrecspec + ")";
            if (!db->StandardSQL(req))
            {
                db->rollback();
                UpMessageBox::Watch(this,tr("Impression annulée"));
                return;
            }
        }
        db->commit();
        accept();
    }
    else if(m_mode == RevoirRemisesPrecs)
        ImprimerRemise(ui->RemisePrecsupComboBox->currentData().toMap()["idRemise"].toInt());
}


void dlg_remisecheques::ItemChequeARemettreClicked(int A, int B)
{
    if (B == 0)
    {
        m_bloquecellchanged = false;

        QTableWidgetItem    *pItem0                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem1                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem2                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem3                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem4                 = new QTableWidgetItem;
        QDoubleValidator    *val                    = new QDoubleValidator(this);
        QLineEdit           *LigneMontant2          = new QLineEdit();
        QLineEdit           *NoLigne                = new QLineEdit();
        int                 i                       = ui->ChequesEnAttenteupTableWidget->rowCount();
        QString             idRec                   = ui->ListeChequesupTableWidget->item(A,5)->text();
        QString             RecSpec                 = ui->ListeChequesupTableWidget->item(A,6)->text();
        QFontMetrics        fm(qApp->font());

        ui->ListeChequesupTableWidget->setCurrentCell(A,B); // nécessaire parce que si on est en mode édition dans la colonne montant, le programme plante
        ui->ChequesEnAttenteupTableWidget->insertRow(i);
        LigneMontant2->setAlignment(Qt::AlignRight);
        val->setDecimals(2);
        LigneMontant2->setValidator(val);
        NoLigne->setAlignment(Qt::AlignRight);
        NoLigne->setFocusPolicy(Qt::NoFocus);
        NoLigne->setStyleSheet("border-style: none;");
        LigneMontant2->setStyleSheet("border-style: none;");

        pItem0->setText("");
        pItem0->setCheckState(Qt::Unchecked);
        ui->ChequesEnAttenteupTableWidget->setItem(i,0,pItem0);
        ui->ChequesEnAttenteupTableWidget->item(i,0)->setToolTip(tr("Cochez la case pour\nmettre ce chèque dans la liste des encaissements"));
        NoLigne->setText(QString::number(i+1));
        ui->ChequesEnAttenteupTableWidget->setCellWidget(i,1,NoLigne);                          // NoLigne
        pItem1->setText(ui->ListeChequesupTableWidget->item(A,2)->text());
        ui->ChequesEnAttenteupTableWidget->setItem(i,2,pItem1);                                 // Tireur
        pItem2->setText(ui->ListeChequesupTableWidget->item(A,3)->text());
        ui->ChequesEnAttenteupTableWidget->setItem(i,3,pItem2);                                 // Banque
        QLineEdit* LigneMontant = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(A,4));
        LigneMontant2->setText(LigneMontant->text());
        ui->ChequesEnAttenteupTableWidget->setCellWidget(i,4,LigneMontant2);                     // Montant
        pItem3->setText(idRec);
        ui->ChequesEnAttenteupTableWidget->setItem(i,5,pItem3);                                 // idRecette
        pItem4->setText(RecSpec);
        ui->ChequesEnAttenteupTableWidget->setItem(i,6,pItem4);                                 // RecSpec

        ui->ChequesEnAttenteupTableWidget->setRowHeight(i,int(fm.height()*1.1));
        ui->ChequesEnAttenteupTableWidget->sortItems(2);

        ui->ListeChequesupTableWidget->removeRow(A);
        // recalcul des numeros de lignes
        for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
        {
            NoLigne = new QLineEdit();
            NoLigne->setText(QString::number(k+1));
            NoLigne->setFocusPolicy(Qt::NoFocus);
            NoLigne->setStyleSheet("border-style: none;");
            ui->ListeChequesupTableWidget->setCellWidget(k,1,NoLigne);
        }
        for (int k = 0; k < ui->ChequesEnAttenteupTableWidget->rowCount(); k++)
        {
            NoLigne = new QLineEdit();
            NoLigne->setText(QString::number(k+1));
            NoLigne->setFocusPolicy(Qt::NoFocus);
            NoLigne->setStyleSheet("border-style: none;");
            ui->ChequesEnAttenteupTableWidget->setCellWidget(k,1,NoLigne);
        }
        RecalculeMontant();
        m_bloquecellchanged = true;

        //mise à jour de la table lignesrecettes ou recettesspeciales
        QString UpdateidRec;
        if (RecSpec == "1")
            UpdateidRec = "update " TBL_RECETTESSPECIALES " set EnAttente = 1 where idRecette = " + idRec;
        else
            UpdateidRec = "update " TBL_RECETTES " set EnAttente = 1 where idRecette = " + idRec;
        db->StandardSQL(UpdateidRec, "void dlg_remisecheques::ItemChequeARemettreClicked(int A, int B)");
    }
}

void dlg_remisecheques::ItemChequeEnAttenteClicked(int A, int B)
{
    if (B == 0)
    {
        m_bloquecellchanged = false;

        QTableWidgetItem    *pItem0                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem1                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem2                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem3                 = new QTableWidgetItem;
        QTableWidgetItem    *pItem4                 = new QTableWidgetItem;
        QLineEdit           *LigneMontant2          = new QLineEdit();
        QLineEdit           *NoLigne                = new QLineEdit;
        int                 i                       = ui->ListeChequesupTableWidget->rowCount();
        QString             idRec                   = ui->ChequesEnAttenteupTableWidget->item(A,5)->text();
        QString             RecSpec                 = ui->ChequesEnAttenteupTableWidget->item(A,6)->text();
        QDoubleValidator    *val                    = new QDoubleValidator(this);

        ui->ChequesEnAttenteupTableWidget->setCurrentCell(A,B); // nécessaire parce que si on est en mode édition dans la colonne montant, le programme plante
        ui->ListeChequesupTableWidget->insertRow(i);
        LigneMontant2->setAlignment(Qt::AlignRight);
        val->setDecimals(2);
        LigneMontant2->setValidator(val);
        NoLigne->setAlignment(Qt::AlignRight);
        NoLigne->setFocusPolicy(Qt::NoFocus);
        NoLigne->setStyleSheet("border-style: none;");
        LigneMontant2->setStyleSheet("border-style: none;");

        pItem0->setText("");
        pItem0->setCheckState(Qt::Checked);
        ui->ListeChequesupTableWidget->setItem(i,0,pItem0);
        ui->ListeChequesupTableWidget->item(i,0)->setToolTip(tr("Décochez la case\npour mettre ce chèqe en attente"));
        pItem1->setText(ui->ChequesEnAttenteupTableWidget->item(A,2)->text());
        ui->ListeChequesupTableWidget->setItem(i,2,pItem1);                                 // Tireur
        pItem2->setText(ui->ChequesEnAttenteupTableWidget->item(A,3)->text());
        ui->ListeChequesupTableWidget->setItem(i,3,pItem2);                                 // Banque
        QLineEdit* LigneMontant = static_cast<QLineEdit*>(ui->ChequesEnAttenteupTableWidget->cellWidget(A,4));
        LigneMontant2->setText(LigneMontant->text());
        ui->ListeChequesupTableWidget->setCellWidget(i,4,LigneMontant2);                     // Montant
        pItem3->setText(idRec);
        ui->ListeChequesupTableWidget->setItem(i,5,pItem3);                                 // idPRecette
        pItem4->setText(RecSpec);
        ui->ListeChequesupTableWidget->setItem(i,6,pItem4);                                 // RecSpec

        QFontMetrics fm(qApp->font());
        ui->ListeChequesupTableWidget->setRowHeight(i,int(fm.height()*1.1));
        ui->ListeChequesupTableWidget->sortItems(2);

        ui->ChequesEnAttenteupTableWidget->removeRow(A);

//        // recalcul des numeros de lignes
        for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
        {
            NoLigne = new QLineEdit();
            NoLigne->setText(QString::number(k+1));
            NoLigne->setFocusPolicy(Qt::NoFocus);
            NoLigne->setStyleSheet("border-style: none;");
            ui->ListeChequesupTableWidget->setCellWidget(k,1,NoLigne);
        }
        for (int k = 0; k < ui->ChequesEnAttenteupTableWidget->rowCount(); k++)
        {
            NoLigne = new QLineEdit();
            NoLigne->setText(QString::number(k+1));
            NoLigne->setFocusPolicy(Qt::NoFocus);
            NoLigne->setStyleSheet("border-style: none;");
            ui->ChequesEnAttenteupTableWidget->setCellWidget(k,1,NoLigne);
        }
        RecalculeMontant();

        //mise à jour de la table lignesrecettes
        QString UpdateidRec;
        if (RecSpec == "1")
            UpdateidRec = "update " TBL_RECETTESSPECIALES " set EnAttente = null where idRecette = " + idRec;
        else
            UpdateidRec = "update " TBL_RECETTES " set EnAttente = null where idRecette = " + idRec;
        db->StandardSQL(UpdateidRec, "void dlg_remisecheques::ItemChequeEnAttenteClicked(int A, int B)");
    }
}

void dlg_remisecheques::MiseEnFormeMontant(int A, int B, int C, int D)
{
    if (B == 4)   // on arrive dans la case montant et on met en mémoire le montant qui y figure
    {
        QLineEdit* Line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(A,B));
        m_valeuravantchangement.setNum(Line->text().toDouble(),'f',2);
    }
    if (D == 4)   // on quitte une case montant et on valide le montant qui y figure sinon, on remet l'ancien
    {
        QLineEdit* Line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(C,4));
        QString b ;
        b = QLocale().toString(QLocale().toDouble(Line->text()),'f',2);
        if (b == "" || QLocale().toDouble(b) <= 0)
            Line->setText(m_valeuravantchangement);
        else
        {
            Line->setText(b);
            RecalculeMontant();
        }
    }
}

void dlg_remisecheques::ChangeCompte()
{
    int id = ui->ComptecomboBox->currentData().toInt();
    Compte *cpt = Datas::I()->comptes->getById(id);
    if (cpt != Q_NULLPTR)
        ui->IntituleComptetextEdit->setText(cpt->nomabrege() + "\n" + cpt->iban());
}

void dlg_remisecheques::ChangeUser()
{
    m_userencours = Datas::I()->users->getById(ui->UserComboBox->currentData().toInt());
    proc->MAJComptesBancaires(m_userencours);
    if (!VoirNouvelleRemise())
        if (!VoirRemisesPrecs())
        {
            RegleComptesComboBox();
            UpMessageBox::Watch(this,tr("Pas d'archive de remises de chèques non plus"));
        }
}

void dlg_remisecheques::RemplirRemisesPrecs(int id)
{
    QFontMetrics        fm(qApp->font());
    QString A;
    disconnect (ui->ListeChequesupTableWidget,     SIGNAL(itemEntered(QTableWidgetItem*)), Q_NULLPTR, Q_NULLPTR);
    QToolTip::showText(cursor().pos(),"");
    ui->ListeChequesupTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->ListeChequesupTableWidget->clearContents();
    QMap<QString, QVariant> MapRemise =  ui->RemisePrecsupComboBox->itemData(id).toMap();
    int idRemise = MapRemise["idRemise"].toInt();
    int idCompte = MapRemise["idCompte"].toInt();
    ui->ComptecomboBox->setCurrentIndex(ui->ComptecomboBox->findData(idCompte));
    QString req = "select idRecette, TireurCheque, BanqueCheque, Montant, null as recspec"
                  " from " TBL_RECETTES
                  " where idRemise = " + QString::number(idRemise);
    req +=        " union"
                  " select idRecette, TireurCheque, BanqueCheque, Montant, 1 as recspec"
                  " from " TBL_RECETTESSPECIALES
                  " where idRemise = " + QString::number(idRemise) +
                  " order by Tireurcheque";
    bool ok = true;
    QList<QVariantList> listremises = db->StandardSelectSQL(req,ok);
    ui->ListeChequesupTableWidget->setRowCount(listremises.size());
    if (listremises.size()>0) {
        QTableWidgetItem *pItem0    = new QTableWidgetItem() ;
        QTableWidgetItem *pItem1    = new QTableWidgetItem() ;
        QTableWidgetItem *pItem2    = new QTableWidgetItem() ;
        QTableWidgetItem *pItem3    = new QTableWidgetItem() ;
        QTableWidgetItem *pItem4    = new QTableWidgetItem() ;
        QTableWidgetItem *pItem5    = new QTableWidgetItem() ;
        UpCheckBox *check           = new UpCheckBox() ;
        for (int i = 0; i < listremises.size(); i++)
        {
            QVariantList remise = listremises.at(i);
            pItem0 = new QTableWidgetItem() ;
            pItem1 = new QTableWidgetItem() ;
            pItem2 = new QTableWidgetItem() ;
            pItem3 = new QTableWidgetItem() ;
            pItem4 = new QTableWidgetItem() ;
            pItem5 = new QTableWidgetItem() ;
            check  = new UpCheckBox() ;

            check->setText("");
            check->setCheckState(Qt::Checked);
            check->setToggleable(false);
            check->setEnabled(false);
            check->setFocusPolicy(Qt::NoFocus);
            ui->ListeChequesupTableWidget->setCellWidget(i,0,check);
            pItem0->setText(QString::number(i+1));
            ui->ListeChequesupTableWidget->setItem(i,1,pItem0);
            A = remise.at(1).toString();                           // Tireur
            pItem1->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,2,pItem1);
            A = remise.at(2).toString();                           // Banque
            pItem2->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,3,pItem2);
            A = QLocale().toString(remise.at(3).toDouble(),'f',2);   // Montant
            pItem3->setText(A);
            pItem3->setTextAlignment(Qt::AlignRight);
            ui->ListeChequesupTableWidget->setItem(i,4,pItem3);
            A = remise.at(0).toString();                           // idRecette
            pItem4->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,5,pItem4);
            A = remise.at(4).toString();                           // recspec
            pItem5->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,6,pItem5);
            ui->ListeChequesupTableWidget->setRowHeight(i,int(fm.height()*1.1));
        }
        ui->ListeChequesupTableWidget->setFocusPolicy(Qt::NoFocus);
        double Total = 0;
        for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
            Total += QLocale().toDouble(ui->ListeChequesupTableWidget->item(k,4)->text());
        ui->TotallineEdit->setText(QString::number(ui->ListeChequesupTableWidget->rowCount()) + tr(" chèques -> ") + QLocale().toString(Total,'f',2) + tr(" euros"));
    }
    req = "select idligne from " TBL_ARCHIVESBANQUE " where idremcheq = " + MapRemise["idRemise"].toString();
    QList<QVariantList> listlignes = db->StandardSelectSQL(req, ok);
    ui->RemisesPrecsPushButton->setEnabled(listlignes.size()==0 && ok);
}

void dlg_remisecheques::RecalculeMontant()
{
    double Total = 0;
    for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
    {
        QLineEdit* Line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(k,4));
        Total = Total + QLocale().toDouble(Line->text());
    }
    ui->TotallineEdit->setText(QString::number(ui->ListeChequesupTableWidget->rowCount()) + tr(" chèques -> ") + QString::number(Total) + tr(" euros"));
}

void dlg_remisecheques::ToolTip(int A, int B)
{
    int col = 5;
    UpTableWidget *tabl = dynamic_cast<UpTableWidget *>(sender());
    if (tabl)   {
        QString RecSpec = tabl->item(A,6)->text();
        if (B == 2 || B == 3)
        {
            QString requete;
            if (RecSpec == "1")
                requete = "SELECT Libelle, Daterecette From " TBL_RECETTESSPECIALES " WHERE idRecette = " + tabl->item(A,col)->text();
            else
                requete = "SELECT PatNom, PatPrenom, " CP_COTATION_ACTES ", " CP_DATE_ACTES " From " TBL_PATIENTS " pat, " TBL_ACTES " act WHERE act." CP_IDACTE_ACTES " in (SELECT " CP_IDACTE_ACTES " FROM "
                        TBL_LIGNESPAIEMENTS " WHERE idRecette = " + tabl->item(A,col)->text() + ") AND pat.idPat = act." CP_IDPAT_ACTES;
            bool ok = true;
            QList<QVariantList> listtips = db->StandardSelectSQL(requete,ok);
            QString ABC;
            for (int i = 0; i < listtips.size();i++)
            {
                if (RecSpec == "1")
                {
                    ABC += listtips.at(i).at(0).toString() + " du " + listtips.at(i).at(1).toDate().toString(tr("dd-MM-yyyy"));
                    break;
                }
                else
                {
                    ABC += listtips.at(i).at(0).toString() + " " + listtips.at(i).at(1).toString() + " " + listtips.at(i).at(2).toString() + " du " + listtips.at(i).at(3).toDate().toString(tr("dd-MM-yyyy"));
                    if (i< listtips.size()-1)
                        ABC += "\n";
                }
            }
            QToolTip::showText(cursor().pos(),ABC);
        }
    }
}

void dlg_remisecheques::TrierChequesEnAttente(int, int B)
{
    if (B == 2 && m_bloquecellchanged) ui->ChequesEnAttenteupTableWidget->sortItems(2);
}

void dlg_remisecheques::TrierListeCheques(int, int B)
{
    if (B == 2 && m_bloquecellchanged) ui->ListeChequesupTableWidget->sortItems(2);
}

bool dlg_remisecheques::VoirRemisesPrecs()
{
    m_mode = RevoirRemisesPrecs;
    ui->ComptecomboBox  ->setEnabled(false);
    RegleComptesComboBox(false);
    ui->ChequesEnAttentelabel           ->setVisible(false);
    ui->ChequesEnAttenteupTableWidget   ->setVisible(false);
    ui->ListeChequesupTableWidget       ->setFixedSize(ui->ListeChequesupTableWidget->width(),580);
    ui->ImprimeupPushButton             ->setIcon(Icons::icLoupe());
    ui->ImprimeupPushButton             ->setText(tr("Revoir"));
    ui->TotallineEdit                   ->move(189,740);
    ui->RemisePrecsupComboBox           ->setVisible(true);
    ui->RemisesPrecsPushButton          ->setText(tr("Supprimer"));
    ui->RemisesPrecsPushButton          ->setIcon(Icons::icPoubelle());

    disconnect (ui->ListeChequesupTableWidget,                 SIGNAL(cellClicked(int, int)),                  Q_NULLPTR, Q_NULLPTR);
    disconnect (ui->ChequesEnAttenteupTableWidget,             SIGNAL(cellClicked(int, int)),                  Q_NULLPTR, Q_NULLPTR);
    disconnect (ui->ListeChequesupTableWidget,                 SIGNAL(currentCellChanged(int,int,int,int)),    Q_NULLPTR, Q_NULLPTR);
    disconnect (ui->ListeChequesupTableWidget,                 SIGNAL(cellChanged(int,int)),                   Q_NULLPTR, Q_NULLPTR);
    disconnect (ui->ChequesEnAttenteupTableWidget,             SIGNAL(cellChanged(int,int)),                   Q_NULLPTR, Q_NULLPTR);
    disconnect (ui->RemisePrecsupComboBox,                     SIGNAL(currentIndexChanged(int)),               Q_NULLPTR, Q_NULLPTR);
    disconnect (ui->RemisesPrecsPushButton,                    SIGNAL(clicked()),                              Q_NULLPTR, Q_NULLPTR);

    QString idlist;
    for( auto it = m_userencours->listecomptesbancaires()->constBegin(); it != m_userencours->listecomptesbancaires()->constEnd(); ++it )
    {
        idlist += QString::number(*it);
        if (it != m_userencours->listecomptesbancaires()->constEnd()-1)
            idlist += ", ";
    }

    bool ok = true;
    QList<QVariantList> listremisesprecedentes = db->StandardSelectSQL("select idRemCheq, RCDate, Montant, idcompte from " TBL_REMISECHEQUES
                                                                          " where idcompte in (" + idlist + ") order by idremcheq desc",ok);
    if (listremisesprecedentes.size() == 0)
    {
        UpMessageBox::Watch(Q_NULLPTR,tr("Pas de remises précédentes"));
        AnnulupPushButton();
        return false;
    }
    ui->RemisePrecsupComboBox->clear();
    for (int i=0; i<listremisesprecedentes.size(); i++)
    {
        QVariantList remise = listremisesprecedentes.at(i);
        QMap<QString,QVariant>  MapRemise;
        MapRemise["idRemise"] = remise.at(0);
        MapRemise["idCompte"] = remise.at(3);
        MapRemise["DateRemise"] = remise.at(1).toDate();
        ui->RemisePrecsupComboBox->addItem("Remise n° " + remise.at(0).toString() + " du " + remise.at(1).toDate().toString(tr("d MMMM yyyy")) + "\t" + QLocale().toString(remise.at(2).toDouble(),'f',2)
                                           ,MapRemise);
    }

    connect(ui->RemisePrecsupComboBox,      QOverload<int>::of(&QComboBox::currentIndexChanged),
                                                                        this,   &dlg_remisecheques::RemplirRemisesPrecs);
    connect(ui->RemisesPrecsPushButton,     &QPushButton::clicked,      this,   &dlg_remisecheques::CorrigeRemise);
    ui->RemisePrecsupComboBox->setFocus();
    RemplirRemisesPrecs(0);
    return true;
}

//-------------------------------------------------------------------------------------
// Gestion des touches ...
//-------------------------------------------------------------------------------------
void dlg_remisecheques::keyPressEvent ( QKeyEvent * event )
{
    switch (event->key())
    {
    case Qt::Key_Escape:
    {
        QKeyEvent *newevent = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Return , Qt::NoModifier);
        QCoreApplication::postEvent (focusWidget(), newevent);
        QWidget::eventFilter(focusWidget(), newevent);
        break;
    }
    default:
        break;
    }
}

void dlg_remisecheques::closeEvent(QCloseEvent *event)
{
    proc->settings()->setValue("PositionsFiches/PositionRemiseCheques", saveGeometry());
    event->accept();
}

//-------------------------------------------------------------------------------------
// Gestion des évènements
//-------------------------------------------------------------------------------------
bool dlg_remisecheques::eventFilter(QObject *obj, QEvent *event) // A REVOIR
{
    if (event->type() == QEvent::KeyPress )
    {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if ((keyEvent->key()==Qt::Key_Return || keyEvent->key() == Qt::Key_Enter) && keyEvent->modifiers() == Qt::NoModifier)
        {
            if (obj == ui->ListeChequesupTableWidget)
            {
                if (ui->ListeChequesupTableWidget->currentColumn() == 3)
                {
                    int AD = ui->ListeChequesupTableWidget->currentRow();
                    if (AD <  ui->ListeChequesupTableWidget->rowCount()-1)
                        ui->ListeChequesupTableWidget->setCurrentCell(AD + 1,3);
                    else
                        ui->ListeChequesupTableWidget->setCurrentCell(0,3);
                    return false;
                }
                else
                {
                    QKeyEvent *newevent = new QKeyEvent ( QEvent::KeyPress, Qt::Key_Tab , Qt::NoModifier);
                    QCoreApplication::postEvent (obj, newevent);
                    return QWidget::eventFilter(obj, newevent);
                }
            }
        }
    }
    return QWidget::eventFilter(obj, event);
}

bool dlg_remisecheques::initOK() const
{
    return m_initok;
}

/*----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
LES FONCTIONS ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------*/
bool dlg_remisecheques::VoirNouvelleRemise()
{
    m_mode = NouvelleRemise;
    ui->ComptecomboBox  ->setEnabled(m_mode == NouvelleRemise);
    ui->UserComboBox    ->setEnabled(m_mode == NouvelleRemise);

        RegleComptesComboBox();
        ui->ChequesEnAttentelabel           ->setVisible(true);
        ui->ChequesEnAttenteupTableWidget   ->setVisible(true);
        ui->ListeChequesupTableWidget       ->setFixedSize(ui->ListeChequesupTableWidget->width(),360);
        ui->TotallineEdit                   ->move(189,520);
        ui->AnnulupPushButton               ->setUpButtonStyle(UpPushButton::ANNULBUTTON, UpPushButton::Small);
        ui->ImprimeupPushButton             ->setUpButtonStyle(UpPushButton::NORMALBUTTON, UpPushButton::Small);
        ui->ImprimeupPushButton             ->setIcon(Icons::icImprimer());
        ui->ImprimeupPushButton             ->setText("Imprimer");
        ui->ImprimeupPushButton             ->setShortcut(QKeySequence("Meta+Return"));
        ui->RemisePrecsupComboBox           ->setVisible(false);
        ui->RemisesPrecsPushButton          ->setText(tr("Remises\nprécédentes"));
        ui->RemisesPrecsPushButton          ->setIcon(Icons::icArchive());
        ui->RemisesPrecsPushButton          ->setUpButtonStyle(UpPushButton::NORMALBUTTON, UpPushButton::Mid);
        ui->RemisesPrecsPushButton          ->setEnabled(true);

        m_bloquecellchanged = true;

        QString req;
        //1, on recherche les chèques à déposer
        req =   "SELECT idRecette, TireurCheque, BanqueCheque, Montant, null as recspec  FROM " TBL_RECETTES " pai"
                " WHERE pai.idRecette in (SELECT lig.idRecette FROM " TBL_LIGNESPAIEMENTS " lig WHERE lig.idActe in"
                " (SELECT act." CP_IDACTE_ACTES " FROM " TBL_ACTES " act WHERE " CP_IDUSERCOMPTABLE_ACTES " = " + QString::number(m_userencours->id()) + "))"
                " AND pai.IdRemise IS NULL"
                " AND EnAttente IS NULL"
                " AND ModePaiement = 'C'";
        req +=  " union"
                " SELECT idRecette, TireurCheque, BanqueCheque, Montant, 1 as recspec  FROM " TBL_RECETTESSPECIALES
                " WHERE IdRemise IS NULL"
                " AND EnAttente IS NULL"
                " AND Paiement = 'C'";
        req +=  " ORDER BY TireurCheque";
        //qDebug() << req;
        bool ok = true;
        QList<QVariantList> listchequesaremettre = db->StandardSelectSQL(req,ok);

        //1, on recherche les chèques à déposer mais dont le tireur à indiqué qu'il souhaitait qu'on attende pour le remettre en banque
        req =   "SELECT idRecette, TireurCheque, BanqueCheque, Montant, null as recspec FROM " TBL_RECETTES " pai"
                " WHERE pai.idRecette in (SELECT lig.idRecette FROM " TBL_LIGNESPAIEMENTS " lig WHERE lig.idActe in"
                " (SELECT act." CP_IDACTE_ACTES " FROM " TBL_ACTES " act WHERE " CP_IDUSERCOMPTABLE_ACTES " = " + QString::number(m_userencours->id()) +"))"
                " AND pai.IdRemise IS NULL"
                " AND EnAttente IS NOT NULL"
                " AND ModePaiement = 'C'";
        req +=  " union"
                " SELECT idRecette, TireurCheque, BanqueCheque, Montant, 1 as recspec  FROM " TBL_RECETTESSPECIALES
                " WHERE IdRemise IS NULL"
                " AND EnAttente IS NOT NULL"
                " AND Paiement = 'C'"
                " ORDER BY TireurCheque";
        //qDebug() << req; // durée = 0,4s en moyenne
        QList<QVariantList> listchequesenattente = db->StandardSelectSQL(req,ok);


        if (listchequesaremettre.size() == 0 && listchequesenattente.size() == 0)
        {
            UpMessageBox::Watch(this,tr("Aucune remise de chèques à effectuer!"));
            return false;
        }

        disconnect (ui->ListeChequesupTableWidget,                 SIGNAL(cellClicked(int, int)),                  Q_NULLPTR, Q_NULLPTR);
        disconnect (ui->ChequesEnAttenteupTableWidget,             SIGNAL(cellClicked(int, int)),                  Q_NULLPTR, Q_NULLPTR);
        disconnect (ui->ListeChequesupTableWidget,                 SIGNAL(currentCellChanged(int,int,int,int)),    Q_NULLPTR, Q_NULLPTR);
        disconnect (ui->ListeChequesupTableWidget,                 SIGNAL(cellChanged(int,int)),                   Q_NULLPTR, Q_NULLPTR);
        disconnect (ui->ChequesEnAttenteupTableWidget,             SIGNAL(cellChanged(int,int)),                   Q_NULLPTR, Q_NULLPTR);
        disconnect (ui->RemisePrecsupComboBox,                     SIGNAL(currentIndexChanged(int)),               Q_NULLPTR, Q_NULLPTR);
        disconnect (ui->RemisesPrecsPushButton,                    SIGNAL(clicked()),                              Q_NULLPTR, Q_NULLPTR);

        QTableWidgetItem    *pItem1, *pItem2, *pItem3, *pItem4, *pItem5;
        QString             A;
        QFontMetrics        fm(qApp->font());
        double              Total = 0;

        ui->ListeChequesupTableWidget->clearContents();
        ui->ChequesEnAttenteupTableWidget->clearContents();

        ui->ListeChequesupTableWidget->setRowCount(listchequesaremettre.size());
        ui->ChequesEnAttenteupTableWidget->setRowCount(listchequesenattente.size());

        for (int i = 0; i < listchequesaremettre.size(); i++)
        {
            QVariantList chequearemettre = listchequesaremettre.at(i);
            pItem1 = new QTableWidgetItem() ;
            pItem2 = new QTableWidgetItem() ;
            pItem3 = new QTableWidgetItem() ;
            pItem4 = new QTableWidgetItem() ;
            pItem5 = new QTableWidgetItem() ;

            QLineEdit           *LigneMontant   = new QLineEdit;
            QLineEdit           *NoLigne        = new QLineEdit();
            QDoubleValidator    *val            = new QDoubleValidator(this);
            val->setDecimals(2);
            LigneMontant->setValidator(val);

            LigneMontant->setAlignment(Qt::AlignRight);
            NoLigne->setAlignment(Qt::AlignRight);
            NoLigne->setFocusPolicy(Qt::NoFocus);
            NoLigne->setStyleSheet("border-style: none;");
            LigneMontant->setStyleSheet("border-style: none;");

            pItem4->setText("");
            pItem4->setCheckState(Qt::Checked);
            ui->ListeChequesupTableWidget->setItem(i,0,pItem4);
            ui->ListeChequesupTableWidget->item(i,0)->setToolTip(tr("Décochez la case\npour mettre ce chèqe en attente"));

            NoLigne->setText(QString::number(i+1));
            ui->ListeChequesupTableWidget->setCellWidget(i,1,NoLigne);
            A = chequearemettre.at(1).toString();                           // Tireur

            pItem1->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,2,pItem1);
            A = chequearemettre.at(2).toString();                           // Banque

            pItem2->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,3,pItem2);
            A = QLocale().toString(chequearemettre.at(3).toDouble(),'f',2); // Montant

            LigneMontant->setText(A);
            ui->ListeChequesupTableWidget->setCellWidget(i,4,LigneMontant);
            A = chequearemettre.at(0).toString();                           // idRecette
            pItem3->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,5,pItem3);

            A = chequearemettre.at(4).toString();                           // idrecspec
            pItem5->setText(A);
            ui->ListeChequesupTableWidget->setItem(i,6,pItem5);
            ui->ListeChequesupTableWidget->setRowHeight(i,int(fm.height()*1.1));
        }

        for (int i = 0; i < listchequesenattente.size(); i++)
        {
            QVariantList chequeenattente = listchequesenattente.at(i);
            pItem1 = new QTableWidgetItem() ;
            pItem2 = new QTableWidgetItem() ;
            pItem3 = new QTableWidgetItem() ;
            pItem4 = new QTableWidgetItem() ;
            pItem5 = new QTableWidgetItem() ;

            QLineEdit           *LigneMontant   = new QLineEdit;
            QLineEdit           *NoLigne        = new QLineEdit();
            QDoubleValidator    *val            = new QDoubleValidator(this);
            val->setDecimals(2);
            LigneMontant->setValidator(val);

            LigneMontant->setAlignment(Qt::AlignRight);
            LigneMontant->setFocusPolicy(Qt::NoFocus);
            NoLigne->setAlignment(Qt::AlignRight);
            NoLigne->setFocusPolicy(Qt::NoFocus);
            NoLigne->setStyleSheet("border-style: none;");
            LigneMontant->setStyleSheet("border-style: none;");

            pItem4->setText("");
            pItem4->setCheckState(Qt::Unchecked);
            ui->ChequesEnAttenteupTableWidget->setItem(i,0,pItem4);
            ui->ChequesEnAttenteupTableWidget->item(i,0)->setToolTip(tr("Cochez la case pour\nmettre ce chèque dans la liste des encaissements"));

            NoLigne->setText(QString::number(i+1));
            ui->ChequesEnAttenteupTableWidget->setCellWidget(i,1,NoLigne);
            A = chequeenattente.at(1).toString();                              // Tireur

            pItem1->setText(A);
            ui->ChequesEnAttenteupTableWidget->setItem(i,2,pItem1);
            A = chequeenattente.at(2).toString();                              // Banque

            pItem2->setText(A);
            ui->ChequesEnAttenteupTableWidget->setItem(i,3,pItem2);
            A = QLocale().toString(chequeenattente.at(3).toDouble(),'f',2);    // Montant

            LigneMontant->setText(A);
            ui->ChequesEnAttenteupTableWidget->setCellWidget(i,4,LigneMontant);
            A = chequeenattente.at(0).toString();                              // idCheque

            pItem3->setText(A);
            ui->ChequesEnAttenteupTableWidget->setItem(i,5,pItem3);
            A = chequeenattente.at(4).toString();                              // idrecspec

            pItem5->setText(A);
            ui->ChequesEnAttenteupTableWidget->setItem(i,6,pItem5);
            ui->ChequesEnAttenteupTableWidget->setRowHeight(i,int(fm.height()*1.1));
        }
        for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
        {
            QLineEdit* Line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(k,4));
            Total = Total + QLocale().toDouble(Line->text());
        }
        ui->TotallineEdit->setText(QString::number(ui->ListeChequesupTableWidget->rowCount()) + tr(" chèques -> ") + QLocale().toString(Total,'f',2) + tr(" euros"));
        ui->TotallineEdit->setAlignment(Qt::AlignRight);
        connect (ui->ListeChequesupTableWidget,                 &QTableWidget::cellClicked,                     this,           &dlg_remisecheques::ItemChequeARemettreClicked);
        connect (ui->ChequesEnAttenteupTableWidget,             &QTableWidget::cellClicked,                     this,           &dlg_remisecheques::ItemChequeEnAttenteClicked);
        connect (ui->ListeChequesupTableWidget,                 &QTableWidget::currentCellChanged,              this,           &dlg_remisecheques::MiseEnFormeMontant);
        connect (ui->ListeChequesupTableWidget,                 &QTableWidget::cellChanged,                     this,           &dlg_remisecheques::TrierListeCheques);
        connect (ui->ChequesEnAttenteupTableWidget,             &QTableWidget::cellChanged,                     this,           &dlg_remisecheques::TrierChequesEnAttente);
        connect (ui->RemisesPrecsPushButton,                    &QPushButton::clicked,                          this,           &dlg_remisecheques::VoirRemisesPrecs);
        ChangeCompte();
    return true;
}

/*---------------------------------------------------------------------------------
    Impression remise de chèques
-----------------------------------------------------------------------------------*/
bool dlg_remisecheques::ImprimerRemise(int idRemise)
{
    bool AvecDupli   = false;
    bool AvecPrevisu = proc->ApercuAvantImpression();
    bool AvecNumPage = true;

    QString LigneChq;

    //--------------------------------------------------------------------
    // Préparation de l'état "bordereau de remise" dans un QplainTextEdit
    //--------------------------------------------------------------------
    double  gtotalMontRemise    = 0;
    int     gtotalNbrePieces    = 0;
     int iduser = ui->UserComboBox->currentData().toInt();
    QDate date;
    QString req;
    int id = ui->ComptecomboBox->currentData().toInt();
    Compte *cpt = Datas::I()->comptes->getById(id);

    if (m_mode == RevoirRemisesPrecs) {
        QMap<QString, QVariant> MapRemise =  ui->RemisePrecsupComboBox->currentData().toMap();
        date = MapRemise["DateRemise"].toDate();
        AvecPrevisu = true;
    }
    else if (m_mode == NouvelleRemise) {
        iduser      = m_userencours->id();
        date        = QDate::currentDate();
    }

    //création de l'entête
    QString EnTete;
    if (iduser == -1) return false;
    User *userEntete = Datas::I()->users->getById(iduser);
    if(userEntete == Q_NULLPTR)
        return false;
    EnTete = proc->CalcEnteteImpression(date, userEntete).value("Norm");
    if (EnTete == "") return false;

    EnTete.replace("{{TITRE1}}"            , Datas::I()->banques->getById(cpt->idBanque())->nomabrege().toUpper());
    EnTete.replace("{{PRENOM PATIENT}}"    , "");
    EnTete.replace("{{NOM PATIENT}}"       , cpt->intitulecompte());
    EnTete.replace("{{TITRE}}"             , "Compte " + cpt->iban());
    EnTete.replace("{{DDN}}"               , "<font color = \"" COULEUR_TITRES "\">Remise de chèques n° " + QString::number(idRemise) + "</font>");

    // création du pied
    QString Pied = proc->CalcPiedImpression(userEntete);
    if (Pied == "") return false;

    // creation du corps
    double c = CORRECTION_td_width;
    QString lignecheque = "<table width=\"" + QString::number(int(c*490)) + "\" border=\"1\" cellspacing=\"0\" cellpadding=\"5\">"
            "<tr>"
              "<td width=\"" + QString::number(int(c*200)) + "\" ><span style=\"font-size:9pt;\">{{NOM PATIENT}}</span></td>"
              "<td width=\"" + QString::number(int(c*180)) + "\" ><span style=\"font-size:9pt;\">{{NOM BANQUE}}</span></td>"
              "<td width=\"" + QString::number(int(c*110)) + "\" ><div align=\"right\"><span style=\"font-size:9pt;\">{{MONT REGLT}}</span></div></td>"
          "</tr>"
          "</table>";

    QString texteordo;
    for (int k = 0; k < ui->ListeChequesupTableWidget->rowCount(); k++)
    {
        // Remplacement des variables par les valeurs lues.
        LigneChq = lignecheque;
        LigneChq.replace("{{NOM PATIENT}}", ui->ListeChequesupTableWidget->item(k,2)->text());
        LigneChq.replace("{{NOM BANQUE}}", ui->ListeChequesupTableWidget->item(k,3)->text());
        if (m_mode == RevoirRemisesPrecs) {
            LigneChq.replace("{{MONT REGLT}}", ui->ListeChequesupTableWidget->item(k,4)->text());
            gtotalMontRemise += QLocale().toDouble(ui->ListeChequesupTableWidget->item(k,4)->text());
        }
        else if (m_mode == NouvelleRemise) {
            QLineEdit *line = static_cast<QLineEdit*>(ui->ListeChequesupTableWidget->cellWidget(k,4));
            LigneChq.replace("{{MONT REGLT}}", line->text());
            gtotalMontRemise += QLocale().toDouble(line->text());
        }
        texteordo += LigneChq;
        gtotalNbrePieces ++;
    }
    texteordo += "<table width=\"" + QString::number(int(c*490)) + "\" border=\"0\" cellspacing=\"0\" cellpadding=\"5\">"
            "<tr>"
              "<td width=\"" + QString::number(int(c*490)) + "\" ><div align=\"right\"><span style=\"font-size:10pt;font-weight:bold\">{{TOTAL REMISE}}</span></div></td>"
            "</tr>"
            "</table>";
    QString totalchq = QString::number(gtotalNbrePieces) + " chèque";
    if (gtotalNbrePieces>1) totalchq += "s";
    texteordo.replace("{{TOTAL REMISE}}", tr("TOTAL - ") + totalchq + tr(" en euros - ") + QString::number(gtotalMontRemise,'f',2));
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setHtml(texteordo);

    bool a = proc->Imprime_Etat(textEdit, EnTete, Pied,
                       proc->TaillePieddePage(), proc->TailleEnTete(), proc->TailleTopMarge(),
                       AvecDupli, AvecPrevisu, AvecNumPage);
    delete textEdit;
    return a;
}

/*---------------------------------------------------------------------------------
    Reconstruit la liste des utilisateurs du combobox
-----------------------------------------------------------------------------------*/
void dlg_remisecheques::ReconstruitListeUsers()
{
    ui->UserComboBox->clear();
    //on reconstruit la liste des users comptables qui ont des chèques en attente
    map_comptables    = Datas::I()->users->comptables();
    map_comptablesavecchequesenattente    = new QMap<int, User*>();

    QString req = "SELECT distinct iduser from " TBL_RECETTES " WHERE IdRemise IS NULL AND ModePaiement = 'C'";
    bool ok = true;
    QList<QVariantList> listiduser = db->StandardSelectSQL(req,ok);
    QListIterator<QVariantList> itusr(listiduser);
    while (itusr.hasNext()) {
        QVariantList listitem = itusr.next();
        if (listitem.at(0).toInt() > 0)
        {
            User *user = Datas::I()->users->getById(listitem.at(0).toInt());
            map_comptablesavecchequesenattente->insert(user->id(), user);
            ui->UserComboBox->addItem(user->login(), user->id() );
        }
    }

    if (map_comptablesavecchequesenattente->count()<1)
    {
        UpMessageBox::Watch(Q_NULLPTR, tr("Pas de remise de chèque en attente"));
        m_initok = false;
        return;
    }
    m_userencours = Datas::I()->users->userconnected();
    //on positionne le combobox sur le comptable de l'utilisateur s'il en a un, sinon sur le premier de la liste
    if (Datas::I()->users->getById(m_userencours->idcomptable()) != Q_NULLPTR)
    {
        auto itusr = map_comptablesavecchequesenattente->find(m_userencours->id());
        if(itusr != map_comptablesavecchequesenattente->end())
            ui->UserComboBox->setCurrentIndex(ui->UserComboBox->findData(m_userencours->id()));
    }
    else
    {
        ui->UserComboBox->setCurrentIndex(0);
        int idusr = ui->UserComboBox->currentData().toInt();
        m_userencours = Datas::I()->users->getById(idusr);
        proc->MAJComptesBancaires(m_userencours);
    }
}

void dlg_remisecheques::RegleComptesComboBox(bool ActiveSeult)
{
    ui->ComptecomboBox->clear();
    foreach (int id, *m_userencours->listecomptesbancaires())
    {
        Compte *cpt = Datas::I()->comptes->getById(id);
        if (cpt != Q_NULLPTR)
        {
            if (ActiveSeult)
            {
                if (!cpt->isDesactive())
                    ui->ComptecomboBox->addItem(cpt->nomabrege(), QString::number(cpt->id()) );
            }
            else
                ui->ComptecomboBox->addItem(cpt->nomabrege(), QString::number(cpt->id()) );
        }
    }
    ui->ComptecomboBox->setCurrentIndex(ui->ComptecomboBox->findData(m_userencours->idcomptepardefaut()));
}
