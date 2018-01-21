#include "music.h"
#include <iostream>
#include <QDebug>

MUSIC::MUSIC(QWidget *parent):QMediaPlayer(parent)
{
    main_parent = parent;
    layout_container = new QWidget;
    layout = new QHBoxLayout;
    v_layout = new QVBoxLayout;
    total_dur = new QLabel;
    cur_dur = new QLabel;
    song_name = new QLabel;
    song_name->setStyleSheet("color: blue");
    progress_slider = new QSlider(Qt::Horizontal);
    connect(progress_slider, SIGNAL(sliderMoved(int)), this, SLOT(forward(int)));
    main_player = new QMediaPlayer(this);
    connect(main_player, SIGNAL(durationChanged(qint64)), this, SLOT(set_total_dur(qint64)));
    connect(main_player, SIGNAL(positionChanged(qint64)), this, SLOT(set_cur_dur(qint64)));

    playlist = new QMediaPlaylist(main_player);
    connect(playlist, SIGNAL(currentMediaChanged(QMediaContent)), this, SLOT(set_song_name(QMediaContent)));
    layout->addWidget(cur_dur);
    layout->addWidget(progress_slider);
    layout->addWidget(total_dur);
    v_layout->addLayout(layout);
    v_layout->addWidget(song_name);
    layout_container->setLayout(v_layout);
}

void MUSIC::play_or_pause()
{
    if(main_player->state() == QMediaPlayer::PlayingState)
        main_player->pause();
    else
        main_player->play();
}

void MUSIC::set_repeat(REPEAT_VAL REP)
{
    switch (REP)
    {
        case REPEAT::ALL_TRACK:
                playlist->setPlaybackMode(QMediaPlaylist::Loop);
                break;

        case REPEAT::CURRENT_TRACK:
                playlist->setPlaybackMode(QMediaPlaylist::CurrentItemInLoop);
                break;

        case REPEAT::NO_REPEAT:
                playlist->setPlaybackMode(QMediaPlaylist::Sequential);
                break;
    }
}

void MUSIC::set_total_dur(qint64 dur)
{
    int total = dur/1000;
    int seconds = total % 60;
    int minuts = total / 60;
    QString total_t = QString::number(minuts) + ":" + QString::number(seconds);
    total_dur->setText(total_t);
    progress_slider->setMaximum(dur/1000);
    progress_slider->setValue(0);
}

void MUSIC::set_cur_dur(qint64 dur)
{
    int con = dur/1000;
    int minuts = con / 60;
    int seconds = con % 60;
    QString consumed_t = QString::number(minuts) + ":" + QString::number(seconds);
    cur_dur->setText(consumed_t);
    progress_slider->setValue(dur/1000);
}

void MUSIC::set_song_name(QMediaContent con)
{
    song_name->setText(con.canonicalUrl().fileName());
}

void MUSIC::forward(int dur)
{
    main_player->setPosition(dur*1000);
}

void MUSIC::play_this(QStringList list)
{
    playlist->clear();
    for(int i=0; i<list.size(); ++i)
    {
        playlist->addMedia(QUrl::fromLocalFile(list[i]));
    }
    main_player->setPlaylist(playlist);
    main_player->play();
}

void MUSIC::play_next()
{
    playlist->next();
}

void MUSIC::play_prev()
{
    playlist->previous();
}

MUSIC::~MUSIC()
{
    delete layout_container;
    delete main_player;
}
