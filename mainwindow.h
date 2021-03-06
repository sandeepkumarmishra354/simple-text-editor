#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#define DO_NOT_COMPILE 0
#define COMPILE(file_to_compile,file_to_run) compiler(file_to_compile,file_to_run)
#define DO_NOT_INTERPRET 0
#define NOT_INITIALISED nullptr
#define INTERPRET(file_to_interpret) interpreter(file_to_interpret)

#include <QMainWindow>
#include <QTextEdit>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QToolBar>
#include <QLineEdit>
#include <QList>
#include <QLabel>
#include <QHBoxLayout>
#include <signal.h>
#include "music.h"

void sig_handle(int); // handle SIGINT (ctrl+c)

class EDITOR:public QMainWindow
{
    Q_OBJECT

    private:
        MUSIC *m_player = NOT_INITIALISED;
        QTextEdit *text_container;
        QTextEdit::ExtraSelection *line_highlight;
        QList<QTextEdit::ExtraSelection> *highlight;
        QLineEdit *search_text;
        QVBoxLayout *layout;
        QWidget *main_widget;
        QMenu *file_menu, *edit_menu, *build_menu;
        QAction *build_run_action; //*run_action;
        QToolBar *tool_bar;
        QAction *music_action, *music_stop, *play_pause_action;
        QAction *next, *prev, *repeat;
        // menu for color choosing
        QMenu *set_color_subMenu;
        QMenu *red_color_subMenu,*pink_color_subMenu,*purple_color_subMenu,*blue_color_subMenu;
        // file menu actions
        QAction *open, *save, *new_file, *quit_action, *close_action;
        // set_color_subMenu actions color
        // red type
        QAction *color_ffcdd2,*color_ef9a9a,*color_e57373;
        // pink type
        QAction *color_e91e63,*color_c2185b,*color_c51162;
        // purple type
        QAction *color_ce93d8,*color_9c27b0,*color_aa00ff;
        // blue type
        QAction *color_7986cb,*color_3f51b5,*color_1a237e;
        // default color action
        QAction *default_color;

        QMenu *about_menu;
        QAction *about_qt, *about_this;
        QStatusBar *status_bar;
        QLabel *current_line, *total_line;
        QHBoxLayout *status_layout;

        std::string file;
        QString title;
        QString file_types;
        size_t is_cpp, is_c;
        size_t is_py;
        bool ply = false;
        bool already = false;
        char *content = nullptr;
        bool something_changed = false;
        bool create_new = false;
        bool repeat_all, repeat_current, no_repeat;

        // private functions
        void create_action();
        void create_color_menu();
        void create_color_action();
        void initialize_file_types();
        void save_with_new_name();
        void save_with_previous_name();
        void set_total_line_num();
        void compiler(std::string, std::string);
        void interpreter(std::string);
        std::string run_command(std::string);
        void display_error(std::string);
        void check_file_extension(QString);
        void closeEvent(QCloseEvent *);

    public:
        EDITOR(QWidget *parent = 0);
        ~EDITOR();
        void load_from_open_with_option(char *);

    private slots:
        void open_file();
        void load_file();
        void save_file();
        void close_file();
        void text_changed();
        void new_file_create();
        void quit_triggered();
        void search(QString);
        void set_current_line_num();
        void build_and_run_file();
        void about_us();
        // red color slots
        void set_color_ffcdd2();
        void set_color_ef9a9a();
        void set_color_e57373();
        // pink color slots
        void set_color_e91e63();
        void set_color_c2185b();
        void set_color_c51162();
        // purple color slots
        void set_color_ce93d8();
        void set_color_9c27b0();
        void set_color_aa00ff();
        // blue color slots
        void set_color_7986cb();
        void set_color_3f51b5();
        void set_color_1a237e();
        // default color slot
        void set_default_color();
        // music slots
        void play_music();
        void stop_music();
        void play_pause();
        void play_next();
        void play_prev();
        void set_repeat();

};

#endif
