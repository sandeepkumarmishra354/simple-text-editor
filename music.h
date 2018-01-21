#ifndef BUILD_OUTPUT_H
#define BUILD_OUTPUT_H
#define REPEAT_VAL short int

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QMediaPlaylist>
#include <QSlider>
#include <QLabel>
#include <QList>

class MUSIC:public QMediaPlayer
{
    Q_OBJECT

    private:
        QMediaPlayer *main_player;
        QMediaPlaylist *playlist;
        QSlider *progress_slider;
        QLabel *total_dur, *cur_dur, *song_name;
        QHBoxLayout *layout;
        QVBoxLayout *v_layout;
        QWidget *main_parent, *layout_container;

    public:
        MUSIC(QWidget *parent = 0);
        ~MUSIC();
        void play_this(QStringList);
        QWidget *get_layout() { return layout_container; }
        void play_or_pause();
        void play_next();
        void play_prev();

        enum REPEAT
        {
            ALL_TRACK, CURRENT_TRACK, NO_REPEAT
        };

        void set_repeat(REPEAT_VAL);

    private slots:
        void set_total_dur(qint64);
        void set_cur_dur(qint64);
        void set_song_name(QMediaContent);
        void forward(int);
};

#endif // BUILD_OUTPUT_H
