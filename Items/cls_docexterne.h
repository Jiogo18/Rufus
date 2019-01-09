/* (C) 2018 LAINE SERGE
This file is part of Rufus.

Rufus is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Rufus is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Rufus. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CLS_DOCEXTERNE_H
#define CLS_DOCEXTERNE_H

#include "cls_item.h"

class DocExterne : public Item
{
private:
    int m_idimpression, m_iduser, m_importance, m_idpatient, m_compression, m_useremetteur;
    QString m_typedoc, m_titre, m_soustypedoc, m_textentete, m_textcorps, m_textorigine, m_textpied, m_formatdoc, m_lienversfichier;
    QDateTime m_dateimpression;
    bool m_ald;
    bool m_isAllLoaded = false;

public:
    explicit DocExterne(QJsonObject data = {}, QObject *parent = Q_NULLPTR);
    void setData(QJsonObject data);

    int id() const;
    int idpatient() const;
    int iduser() const;
    QString typedoc() const;
    QString soustypedoc() const;
    QString titre() const;
    QString textentete() const;
    QString textcorps() const;
    QString textorigine() const;
    QString textpied() const;
    QDateTime date() const;
    QString lienversfichier() const;
    bool isALD() const;
    int useremetteur() const;
    QString format() const;
    int importance() const;
    int compression() const;
    bool isAllLoaded() const;

    void setDate(QDateTime date);
    void setimportance(int imptce);
    void setAllLoaded(bool allloaded);
};
#endif // CLS_DOCEXTERNE_H
