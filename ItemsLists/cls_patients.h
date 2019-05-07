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

#ifndef CLS_PATIENTS_H
#define CLS_PATIENTS_H

#include <QObject>
#include <cls_patient.h>
#include "database.h"

class Patients
{

public:
    explicit Patients();

    //GETTER
    QList<Patient*> *patients() const;


    Patient* getById(int id, bool all = false);                                             /*! charge les données du patient corresondant à l'id
                                                                                             * \brief Patients::getById
                                                                                             * \param id l'id du patient recherché
                                                                                             * \param all =false  -> ne charge que les données d'identité - =true -> charge les données sociales et médicales
                                                                                             * \return Q_NULLPTR si aucun patient trouvé
                                                                                             * \return Patient* le patient correspondant à l'id
                                                                                             */

    bool isfull();

    bool add(Patient *patient);
    void addList(QList<Patient*> listpatientss);
    void remove(Patient* patient);
    void clearAll();
    void initListeAll(QString nom = "", QString prenom = "", bool filtre = false);          /*! crée une liste de patients
                                                                                            * \param patnom filtrer sur le nom de patient
                                                                                            * \param patprenom filtrer sur le prénom de patient
                                                                                            * \param le filtre se fait sur des valeurs aprrochantes */

    void initListeByDDN(QDate DDN = QDate());                                               /*! crée une liste de tous les patients pour une date de naissance
                                                                                            * \param DDN la date de naissance */
    private:
    QList<Patient*> *m_patients;                                                            //!< une liste de patients
    bool    m_full;                                                                         //!nlanliste de patients contient tous les patients de la base
};

#endif // CLS_PATIENTS_H
