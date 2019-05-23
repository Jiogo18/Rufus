#include "cls_ville.h"

/*
 * Ville
*/
Ville::Ville(QJsonObject data, QObject *parent) : Item(parent)
{
    setData( data );
}

int Ville::id() const { return m_id; }
QString Ville::codePostal() const { return m_codePostal; }
QString Ville::nom() const { return m_nom; }

void Ville::setData(QJsonObject data)
{
    if( data.isEmpty() )
        return;

    setDataInt(data, "ville_id", m_id);
    setDataString(data, "codePostal", m_codePostal);
    setDataString(data, "ville", m_nom);
}


