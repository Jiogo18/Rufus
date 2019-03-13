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

#include "dlg_gestioncotations.h"
#include "ui_dlg_gestioncotations.h"

dlg_gestioncotations::dlg_gestioncotations(QString TypeActe, QString Mode , QString CodeActe, QWidget *parent) :
    UpDialog(parent),
    ui(new Ui::dlg_gestioncotations)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    dlglayout()->insertWidget(0,ui->widget);
    dlglayout()->insertWidget(0,ui->TypeActegroupBox);
    AjouteLayButtons(UpDialog::ButtonOK);
    dlglayout()->setSizeConstraint(QLayout::SetFixedSize);
    gTypeActe   =  AssocCCAM;
    if      (TypeActe   == "CCAM")                gTypeActe   = CCAM;
    else if (TypeActe   == "Assoc")               gTypeActe   = AssocCCAM;
    else if (TypeActe   == "HorsNomenclature")    gTypeActe   = HorsNomenclature;
    gMode       = Creation;
    if      (Mode       == "Creation")            gMode       = Creation;
    else if (Mode       == "Modification")        gMode       = Modification;
    gCodeActe   = CodeActe;

    QDoubleValidator    *val                    = new QDoubleValidator(this);
    ui->TarifOPTAMupLineEdit                    ->setValidator(val);
    ui->TarifNonOPTAMupLineEdit                 ->setValidator(val);
    ui->TarifPratiqueupLineEdit                 ->setValidator(val);
    ui->CodeActeupLineEdit                      ->setText(CodeActe);

    connect(OKButton,   &QPushButton::clicked,  this, [=] {if (VerifFiche()) accept();});
}

dlg_gestioncotations::~dlg_gestioncotations()
{
    delete ui;
}

void dlg_gestioncotations::Initialise()
{
    //TODO : SQL
    db                      = DataBase::getInstance();
    gidUser                 = db->getUserConnected()->id();
    gSecteurUser            = db->getUserConnected()->getSecteur();
    if (gTypeActe != HorsNomenclature)
    {
        ui->CodeActeupLineEdit    ->setValidator(new QRegExpValidator(Utils::rgx_cotation,this));
        ui->CodeActelabel->setText(tr("Code (Majuscules)"));
    }

    QString req = "select distinct typeacte from " NOM_TABLE_COTATIONS " where CCAM = ";
    ui->CodeActeupLineEdit->setEnabled(gMode==Creation);
    switch (gTypeActe) {
    case CCAM:
        ui->CCAMradioButton         ->setChecked(true);
        ui->AssocradioButton        ->setEnabled(false);
        ui->HorsCCAMradioButton     ->setEnabled(false);
        ui->TarifOPTAMupLineEdit    ->setVisible(true);
        ui->TarifNonOPTAMupLineEdit ->setVisible(true);
        ui->TarifOPTAMlabel         ->setVisible(true);
        ui->TarifNonOPTAMlabel      ->setVisible(true);
        ui->TarifPratiquelabel      ->setVisible(gSecteurUser>1);
        ui->TarifPratiqueupLineEdit ->setVisible(gSecteurUser>1);
        req += "1";
        break;
    case AssocCCAM:
        ui->CCAMradioButton         ->setEnabled(false);
        ui->AssocradioButton        ->setChecked(true);
        ui->HorsCCAMradioButton     ->setEnabled(false);
        ui->TarifNonOPTAMlabel      ->setVisible(true);
        ui->TarifNonOPTAMupLineEdit ->setVisible(true);
        ui->TarifOPTAMlabel         ->setVisible(true);
        ui->TarifOPTAMupLineEdit    ->setVisible(true);
        ui->TarifPratiquelabel      ->setVisible(gSecteurUser>1);
        ui->TarifPratiqueupLineEdit ->setVisible(gSecteurUser>1);
        req += "2";
        break;
    case HorsNomenclature:
        ui->CCAMradioButton         ->setEnabled(false);
        ui->AssocradioButton        ->setEnabled(false);
        ui->HorsCCAMradioButton     ->setChecked(true);
        ui->TarifOPTAMupLineEdit    ->setVisible(false);
        ui->TarifNonOPTAMupLineEdit ->setVisible(false);
        ui->TarifOPTAMlabel         ->setVisible(false);
        ui->TarifNonOPTAMlabel      ->setVisible(false);
        ui->TarifPratiquelabel      ->setVisible(true);
        ui->TarifPratiqueupLineEdit ->setVisible(true);
        req += "3";
        break;
    default:
        break;
    }

    if (gMode == Creation)
    {
        bool ok;
        QList<QList<QVariant>> listtypacte = db->StandardSelectSQL(req, ok);
        if (ok && listtypacte.size()>0)
        {
            for (int i=0; i<listtypacte.size(); i++)
                glistActes << listtypacte.at(i).at(0).toString();
            QCompleter *complistactes = new QCompleter(glistActes,this);
            complistactes->setCompletionMode(QCompleter::InlineCompletion);
            ui->CodeActeupLineEdit->setCompleter(complistactes);
        }
    }
    else
    {
        ui->CodeActeupLineEdit->setText(gCodeActe);
        req = " select montantoptam, montantnonoptam, montantpratique from " NOM_TABLE_COTATIONS " where idUser = " + QString::number(gidUser) + " and TypeActe = '" + gCodeActe + "'";
        bool ok;
        QList<QVariant> listcot = db->getFirstRecordFromStandardSelectSQL(req, ok);
        if (ok && listcot.size()>0)
        {
            ui->TarifOPTAMupLineEdit    ->setText(QLocale().toString(listcot.at(0).toDouble(),'f',2));
            ui->TarifNonOPTAMupLineEdit ->setText(QLocale().toString(listcot.at(1).toDouble(),'f',2));
            ui->TarifPratiqueupLineEdit ->setText(QLocale().toString(listcot.at(2).toDouble(),'f',2));
        }
    }
}

bool dlg_gestioncotations::VerifFiche()
{
    bool a = true;
    QString req = "";
    QString msg = tr(("Rensegnements incomplets"));
    switch (gTypeActe) {
    case CCAM:
        break;
    case AssocCCAM:
        if (ui->TarifOPTAMupLineEdit->text()=="")
        {
           a = false;
           UpMessageBox::Watch(this, msg, tr("Vous avez oublié de mentionner le tarif OPTAM"));
           ui->TarifOPTAMupLineEdit->setFocus();
           break;
        }
        if (ui->TarifNonOPTAMupLineEdit->text()=="")
        {
           a = false;
           UpMessageBox::Watch(this, msg, tr("Vous avez oublié de mentionner le tarif hors OPTAM"));
           ui->TarifNonOPTAMupLineEdit->setFocus();
           break;
        }
        if (ui->TarifPratiqueupLineEdit->isVisible() && ui->TarifPratiqueupLineEdit->text()=="")
        {
           a = false;
           UpMessageBox::Watch(this, msg, tr("Vous avez oublié de mentionner le tarif pratiqué"));
           ui->TarifPratiqueupLineEdit->setFocus();
           break;
        }
        if (gMode == Creation)
        {
            req = "select idcotation from " NOM_TABLE_COTATIONS " where typeacte = '" + ui->CodeActeupLineEdit->text() + "' and CCAM = 2 and idUser = " + QString::number(gidUser);
            bool ok;
            QList<QVariant> actdata = db->getFirstRecordFromStandardSelectSQL(req, ok);
            if (ok && actdata.size()>0)
            {
                a = false;
                UpMessageBox::Watch(this, msg, tr("Cet acte est déjà enregistré"));
            }
        }
        if (!a)
           break;
        if (gMode == Creation)
        {
            req = "insert into " NOM_TABLE_COTATIONS " (TypeACte, MontantOPTAM, MontantNonOPTAM, MontantPratique, CCAM, idUser) VALUES ('"
                    + ui->CodeActeupLineEdit->text() + "', "
                    + QString::number(QLocale().toDouble(ui->TarifOPTAMupLineEdit->text())) + ", "
                    + QString::number(QLocale().toDouble(ui->TarifNonOPTAMupLineEdit->text())) + ", "
                    + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) + ", 2, "
                    + QString::number(gidUser) + ")";
        }
        else
        {
            req = "update " NOM_TABLE_COTATIONS " set "
                  " MontantOPTAM = "    + QString::number(QLocale().toDouble(ui->TarifOPTAMupLineEdit->text())) + ", " +
                  " MontantOPTAM = "    + QString::number(QLocale().toDouble(ui->TarifNonOPTAMupLineEdit->text())) + ", " +
                  " MontantPratique = " + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) +
                  " where"
                  " idUser = "          + QString::number(gidUser) +
                  " and TypeActe = '"   + ui->CodeActeupLineEdit->text() + "'";
        }
        break;
    case HorsNomenclature:
        if (QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())==0.0)
        {
            a = false;
           UpMessageBox::Watch(this, msg, tr("Vous avez oublié de mentionner le tarif pratiqué"));
           ui->TarifPratiqueupLineEdit->setFocus();
           break;
        }
        if (gMode == Creation)
        {
            req = "select idcotation from " NOM_TABLE_COTATIONS " where typeacte = '" + ui->CodeActeupLineEdit->text() + "' and CCAM = 3 and idUser = " + QString::number(gidUser);
            bool ok;
            QList<QVariant> cotdata = db->getFirstRecordFromStandardSelectSQL(req, ok);
            if (ok && cotdata.size()>0)
            {
                a = false;
                UpMessageBox::Watch(this, msg, tr("Cet acte est déjà enregistré"));
            }
        }
        if (!a)
        {
           break;
        }
        if (gMode == Creation)
        {
            req = "insert into " NOM_TABLE_COTATIONS " (TypeACte, MontantOPTAM, MontantNonOPTAM, MontantPratique, CCAM, idUser) VALUES ('"
                    + ui->CodeActeupLineEdit->text() + "', "
                    + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) + ", "
                    + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) + ", "
                    + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) + ", 3, "
                    + QString::number(gidUser) + ")";
        }
        else
        {
            req = "update " NOM_TABLE_COTATIONS " set "
                  " MontantOPTAM = "    + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) + ", " +
                  " MontantNonOPTAM = " + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) + ", " +
                  " MontantPratique = " + QString::number(QLocale().toDouble(ui->TarifPratiqueupLineEdit->text())) +
                  " where"
                  " idUser = "          + QString::number(gidUser) +
                  " and TypeActe = '"   + ui->CodeActeupLineEdit->text() + "'";
        }
        break;
    default:
        break;
    }
    if (!a)
        return false;
    db->StandardSQL(req);
    return true;
}
