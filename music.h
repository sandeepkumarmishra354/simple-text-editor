#ifndef BUILD_OUTPUT_H
#define BUILD_OUTPUT_H
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QtMultimedia/QMediaPlayer>
#include <QSlider>
#include <QLabel>

class MUSIC:public QMediaPlayer
{
    Q_OBJECT

    private:
        QMediaPlayer *main_player;
        QSlider *progress_slider;
        QLabel *total_dur, *cur_dur;
        QHBoxLayout *layout;
        QVBoxLayout *v_layout;
        QWidget *main_parent, *layout_container;

    public:
        MUSIC(QWidget *parent = 0);
        ~MUSIC();
        void play_this(QString);
        QWidget *get_layout() { return layout_container; }
        void play_or_pause();

    private slots:
        void set_total_dur(qint64);
        void set_cur_dur(qint64);
        void forward(int);
};

#endif // BUILD_OUTPUT_H
