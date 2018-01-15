#include "music.h"
#include <iostream>

MUSIC::MUSIC(QWidget *parent):QMediaPlayer(parent)
{
    main_parent = parent;
    layout_container = new QWidget;
    layout = new QHBoxLayout;
    v_layout = new QVBoxLayout;
    total_dur = new QLabel;
    cur_dur = new QLabel;
    progress_slider = new QSlider(Qt::Horizontal);
    connect(progress_slider, SIGNAL(sliderMoved(int)), this, SLOT(forward(int)));
    main_player = new QMediaPlayer(this);
    connect(main_player, SIGNAL(durationChanged(qint64)), this, SLOT(set_total_dur(qint64)));
    connect(main_player, SIGNAL(positionChanged(qint64)), this, SLOT(set_cur_dur(qint64)));

    layout->addWidget(cur_dur);
    layout->addWidget(progress_slider);
    layout->addWidget(total_dur);
    v_layout->addLayout(layout);
    layout_container->setLayout(v_layout);
}

void MUSIC::play_or_pause()
{
    if(main_player->state() == QMediaPlayer::PlayingState)
        main_player->pause();
    else
        main_player->play();
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

void MUSIC::forward(int dur)
{
    main_player->setPosition(dur*1000);
}

void MUSIC::play_this(QString file_to_play)
{
    main_player->setMedia(QUrl::fromLocalFile(file_to_play));
    main_player->play();
    progress_slider->setToolTip(file_to_play);
}

MUSIC::~MUSIC()
{
    delete layout_container;
    delete main_player;
}
