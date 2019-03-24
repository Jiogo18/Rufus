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

#ifndef PLAYERCONTROLS_H
#define PLAYERCONTROLS_H

#include <QMediaPlayer>
#include <QLabel>
#include <QWidget>
#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QTime>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>
#include "utils.h"


QT_BEGIN_NAMESPACE
class QAbstractButton;
class QAbstractSlider;
QT_END_NAMESPACE

class PlayerControls : public QWidget
{
    Q_OBJECT

public:
    PlayerControls(QWidget *parent = Q_NULLPTR);
    ~PlayerControls();
    void        setPlayer(QMediaPlayer*);
    int         State();

signals:
    void ctrl();
    void recfile();

private slots:
    void playClicked();
    void stopClicked();
    void playSeek(int);
    void recvideo();
    void positionChanged(qint64 progress);
    void updateDurationInfo(qint64 progress);
    QString format(QMediaPlayer *plyr);

private:
    QMediaPlayer    *player;
    QAbstractButton *playButton;
    QAbstractButton *stopButton;
    QAbstractButton *recfileButton;
    QSlider         *slider;
    QLabel          *labelDuration;
    int             state;
};

#endif // PLAYERCONTROLS_H
