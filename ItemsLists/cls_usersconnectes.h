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

#ifndef CLS_USERSCONNECTES_H
#define CLS_USERSCONNECTES_H

#include "cls_itemslist.h"
#include "cls_userconnecte.h"
#include "database.h"

class UsersConnectes : public ItemsList
{
public:
    QMap<int, UserConnecte*> *m_usersconnectes = Q_NULLPTR;  //!< la liste de tous les usersconnectes
    void addList(QList<UserConnecte*> listusr);

public:
    explicit UsersConnectes(QObject *parent = Q_NULLPTR);
    ~UsersConnectes();

    QMap<int, UserConnecte*>* usersconnectes() const;

    UserConnecte* getById(int id);
    void initListe();

    //!> actions sur les enregistrements
    void             SupprimeUserConnecte(UserConnecte *cpt);
    UserConnecte*    CreationUserConnecte();
};

#endif // CLS_USERSCONNECTES_H
