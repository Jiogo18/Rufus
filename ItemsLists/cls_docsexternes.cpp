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

#include "cls_docsexternes.h"

DocsExternes::DocsExternes(QObject *parent) : ItemsList(parent)
{
    m_docsexternes = new QMap<int, DocExterne*>();
}

QMap<int, DocExterne *> *DocsExternes::docsexternes()
{
    return m_docsexternes;
}

/*!
 * \brief docsexternes::getById
 * \param id l'idimpr du Document recherché
 * \return Q_NULLPTR si aucun Document trouvée
 * \return DocExterne* le Document correspondant à l'id
 */
DocExterne* DocsExternes::getById(int id, Item::LOADDETAILS loadDetails, ADDTOLIST addToList)
{
    QMap<int, DocExterne*>::const_iterator itdoc = m_docsexternes->find(id);
    DocExterne *result;
    if( itdoc == m_docsexternes->constEnd() )
            result = new DocExterne();
    else
    {
        result = itdoc.value();
        if (loadDetails == Item::NoLoadDetails)
            return result;
        addToList = ItemsList::NoAddToList;
    }

    if( !result->isAllLoaded() )
    {
        QJsonObject jsonDocExterne = DataBase::I()->loadDocExterneData(id);
        if( jsonDocExterne.isEmpty() )
        {
            delete result;
            return Q_NULLPTR;
        }
        else
            result->setData(jsonDocExterne);
    }
    if( addToList == ItemsList::AddToList)
        add( m_docsexternes, result->id(), result );
    return result;
}

bool DocsExternes::NouveauDocument()
{
    return m_nouveaudocument;
}

void DocsExternes::setNouveauDocumentFalse()
{
    m_nouveaudocument = false;
}

void DocsExternes::setsoustype(DocExterne* docmt, QString soustype)
{
    if (soustype == "")
        soustype = "null";
    else
        soustype = "'" + Utils::correctquoteSQL(soustype) + "'";
    DataBase::I()->StandardSQL("update " TBL_IMPRESSIONS " set soustypedoc = " + soustype + " where idimpression = " + QString::number(docmt->id()));
}

void DocsExternes::addList(QList<DocExterne*> listdocs)
{
    for(QList<DocExterne*>::const_iterator it = listdocs.constBegin(); it != listdocs.constEnd(); ++it )
    {
        DocExterne *doc = const_cast<DocExterne*>(*it);
        if(!m_docsexternes->contains(doc->id()))
            m_nouveaudocument = true;
        add(m_docsexternes, doc->id(), doc);
    }
}

/*!
 * \brief DocsExternes::initListeByPatient
 * Charge l'ensemble des documents externes pour un patient
 * et les ajoute à la classe Patients
 */
void DocsExternes::initListeByPatient(Patient *pat)
{
    clearAll(m_docsexternes);
    addList(DataBase::I()->loadDoscExternesByPatient(pat));
}


void DocsExternes::SupprimeDocument(DocExterne *doc)
{
    DataBase::I()->StandardSQL("delete from " TBL_REFRACTION " where idrefraction = (select idrefraction from " TBL_IMPRESSIONS
                    " where idimpression = " + QString::number(doc->id()) + ")");
    DataBase::I()->StandardSQL("delete from " TBL_ECHANGEIMAGES " where idimpression = " + QString::number(doc->id()));
    DataBase::I()->SupprRecordFromTable(doc->id(), CP_IDIMPRESSION_IMPRESSIONS, TBL_IMPRESSIONS);
    remove(m_docsexternes, doc);
}

DocExterne* DocsExternes::CreationDocument(int idUser, int idPat, QString TypeDoc, QString SousTypeDoc, QString Titre,
                                           QString TextEntete, QString TextCorps, QString TextOrigine, QString  TextPied, QDateTime DateImpression,
                                           QByteArray pdf, bool Compression, QByteArray jpg, QByteArray autre, QString formatautre,
                                           QString lienversfichier, int idRefraction, bool ALD, int UserEmetteur, QString Conclusion,
                                           int EmisRecu, QString FormatDoc, int idLieu, int Importance)
{
    DocExterne *doc = Q_NULLPTR;
    QString idusr           = QString::number(idUser);
    QString idpat           = QString::number(idPat);
    QString typdoc          = (TypeDoc == ""?               "" : "'" + Utils::correctquoteSQL(TypeDoc) + "'");
    QString sstypdoc        = (SousTypeDoc == ""?           "" : "'" + Utils::correctquoteSQL(SousTypeDoc) + "'");
    QString titre           = (Titre == ""?                 "" : "'" + Utils::correctquoteSQL(Titre) + "'");
    QString entete          = (TextEntete == ""?            "" : "'" + Utils::correctquoteSQL(TextEntete) + "'");
    QString corps           = (TextCorps == ""?             "" : "'" + Utils::correctquoteSQL(TextCorps) + "'");
    QString txtorigin       = (TextOrigine == ""?           "" : "'" + Utils::correctquoteSQL(TextOrigine) + "'");
    QString pied            = (TextPied == ""?              "" : "'" + Utils::correctquoteSQL(TextPied) + "'");
    QString date            = (DateImpression.isValid()?    "'" + QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss") + "'" : "'" + DateImpression.toString("yyyy-MM-dd HH:mm:ss") + "'");
    QString comprime        = (!Compression?                "null" : "1");
    QString autreformat     = (formatautre == ""?           "" : "'" + Utils::correctquoteSQL(formatautre) + "'");
    QString lienfichier     = (lienversfichier == ""?       "" : "'" + Utils::correctquoteSQL(lienversfichier) + "'");
    QString idref           = (idRefraction == 0?           "null" : QString::number(idRefraction));
    QString ald             = (!ALD?                        "null" : "1");
    QString idemetteur      = QString::number(UserEmetteur);
    QString conclusion      = (Conclusion == ""?            "" : "'" + Utils::correctquoteSQL(Conclusion) + "'");
    QString emisrecu        = QString::number(EmisRecu);
    QString formatdoc       = (FormatDoc == ""?             "" : "'" + Utils::correctquoteSQL(FormatDoc) + "'");
    QString idlieu          = QString::number(idLieu);
    QString importace       = QString::number(Importance);
/*     bool ok;
    QString req =     "INSERT INTO " TBL_DEPENSES
            " (idUser, DateDep, RefFiscale, Objet, Montant, FamFiscale,"
            " Monnaie, idRec, ModePaiement, Compte, Nocheque, idFacture)"
            " VALUES (" +   idusr + "," +
            date + "," +
            ref + "," +
            objet   + "," +
            montant   + "," +
            fam   + "," +
            monnaie + "," +
            idrec   + "," +
            modpaiement   + "," +
            idcpt   + "," +
            nochq   + "," +
            idfacture + ")";
    qDebug() << req;
    QString MsgErreur           = tr("Impossible d'enregistrer cette dépense");
    DataBase::I()->locktables(QStringList() << TBL_DEPENSES);
    if (!DataBase::I()->StandardSQL(req, MsgErreur))
    {
        DataBase::I()->unlocktables();
        return Q_NULLPTR;
    }
    // Récupération de l'idMotif créé ------------------------------------
    int iddep = DataBase::I()->selectMaxFromTable(CP_IDDEPENSE_DEPENSES, TBL_DEPENSES, ok, tr("Impossible de sélectionner les enregistrements"));
    DataBase::I()->unlocktables();*/
    return doc;
}

