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

#ifndef DONNEESOPHTAPATIENT_H
#define DONNEESOPHTAPATIENT_H

#include "cls_item.h"

class DonneesOphtaPatient : public Item
{
public:
    DonneesOphtaPatient();
    void setData(QJsonObject data);

private:
    int m_idpat;                        //!> idpatient
    QString m_mesure;                   //!> A = autoref R = refraction

    //!< les données de réfraction subjective
    int m_idsubjectif = 0;              //!> le no de l'enregistrement de refraction dans la table
    QDate m_daterefOG;                  //!> date refraction OG
    QString m_distance;                 //!> distance de mesure pour les réfractions
    double m_sphereOD;                  //!> sphereODsubjectif
    double m_cylindreOD;                //!> cylindreODsubjectif
    int m_axecylindreOD;                //!> axecylindreODsubjectif
    QString m_AVLOD;                    //!> AV loin OD
    double m_addVPOD;                   //!> addition VP OD
    QString m_AVPOD;                    //!> AV près OD
    QDate m_daterefOD;                  //!> date refraction OD
    double m_sphereOG;                  //!> spherereOGsubjectif
    double m_cylindreOG;                //!> cylindreOGsubjectif
    int m_axecylindreOG;                //!> axecylindreOGsubjectif
    QString m_AVLOG;                    //!> AV loin OG
    double m_addVPOG;                   //!> addition VP OG
    QString m_AVPOG;                    //!> AV près OG
    int m_ecartIP;                      //!> ecart interpupillaire

    //!< les données mesurées par l'autoref : kératométrie et autoref
    int m_idautoref = 0;                //!> le no de l'enregistrement d'autoref dans la table
    QDate m_dateK;                      //!> date keratométrie
    QString m_origineK;                 //!> origine de la mesure de keratométrie A = autoref T = Topographe
    double m_K1OD = 0;                  //!> K1OD
    double m_K2OD = 0;                  //!> K2OD
    int m_axeKOD = 0;                   //!> AxeKOD
    double m_dioptriesK1OD = 0;         //!> diotries K1OD
    double m_dioptriesK2OD = 0;         //!> diotries K2OD
    double m_dioptriesKOD = 0;          //!> diotries KMOD
    double m_K1OG = 0;                  //!> K1OG
    double m_K2OG = 0;                  //!> K2OG
    int m_axeKOG = 0;                   //!> AxeKOG
    double m_dioptriesK1OG = 0;         //!> diotries K1OG
    double m_dioptriesK2OG = 0;         //!> diotries K2OG
    double m_dioptriesKOG = 0;          //!> diotries KMOG

    double m_sphereODautoref;           //!> sphereODautoref
    double m_cylindreODautoref;         //!> cylindreODautoref
    int m_axecylindreODautoref;         //!> axecylindreODautoref
    QDate m_daterefODautoref;           //!> date de la mesure autoref OD
    double m_sphereOGautoref;           //!> spherereOGautoref
    double m_cylindreOGautoref;         //!> cylindreOGautoref
    int m_axecylindreOGautoref;         //!> axecylindreOGautoref
    QDate m_daterefOGautoref;           //!> date de la mesure autoref OG
    int m_ecartIPautoref;               //!> ecart interpupillaire mesuré par l'autoref

public:
    int idpat() const                   { return m_idpat; }
    QString mesure() const              { return m_mesure; }

    int idsubjectif()  const            { return m_idsubjectif; }
    QString distance() const            { return m_distance; }
    double sphereOD() const             { return m_sphereOD; }
    double cylindreOD() const           { return m_cylindreOD; }
    int axecylindreOD() const           { return m_axecylindreOD; }
    QString AVLOD() const               { return m_AVLOD; }
    double addVPOD() const              { return m_addVPOD; }
    QString AVPOD() const               { return m_AVPOD; }
    QDate daterefOD() const             { return m_daterefOD; }
    double sphereOG() const             { return m_sphereOG; }
    double cylindreOG() const           { return m_cylindreOG; }
    int axecylindreOG() const           { return m_axecylindreOG; }
    QString AVLOG() const               { return m_AVLOG; }
    double addVPOG() const              { return m_addVPOG; }
    QString AVPOG() const               { return m_AVPOG; }
    QDate daterefOG() const             { return m_daterefOG; }
    int ecartIP() const                 { return m_ecartIP; }

    int idautoref() const               { return m_idautoref; }

    bool ismesurekerato() const         { return (m_K1OD > 0 || m_K1OG > 0); }
    QDate dateK() const                 { return m_dateK; }
    QString origineK() const            { return m_origineK; }
    double K1OD() const                 { return m_K1OD; }
    double K2OD() const                 { return m_K2OD; }
    int axeKOD() const                  { return m_axeKOD; }
    double dioptrieK1OD() const         { return m_dioptriesK1OD; }
    double dioptrieK2OD() const         { return m_dioptriesK2OD; }
    double dioptrieKMOD() const         { return m_dioptriesKOD; }
    double K1OG() const                 { return m_K1OG; }
    double K2OG() const                 { return m_K2OG; }
    int axeKOG() const                  { return m_axeKOG; }
    double dioptrieK1OG() const         { return m_dioptriesK1OG; }
    double dioptrieK2OG() const         { return m_dioptriesK2OG; }
    double dioptrieKMOG() const         { return m_dioptriesKOG; }

    double sphereODautoref() const      { return m_sphereODautoref; }
    double cylindreODautoref() const    { return m_cylindreODautoref; }
    int axecylindreODautoref() const    { return m_axecylindreODautoref; }
    QDate daterefODautoref() const      { return m_daterefODautoref; }
    double spherereOGautoref() const    { return m_sphereOGautoref; }
    double cylindreOGautoref() const    { return m_cylindreOGautoref; }
    int axecylindreOGautoref() const    { return m_axecylindreOGautoref; }
    QDate daterefOGautoref() const      { return m_daterefOGautoref; }
    int ecartIPautoref() const          { return m_ecartIPautoref; }
};

#endif // DONNEESOPHTAPATIENT_H
