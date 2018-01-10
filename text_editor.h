#ifndef MAINWINDOW_H
#define MAINWINDOW_H
//#ifndef DO_NOT_COMPILE
#define DO_NOT_COMPILE 0
#define COMPILE(file_to_compile) run_command(file_to_compile)

#include <QMainWindow>
#include <QTextEdit>
#include <QBoxLayout>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QStatusBar>
#include <QToolBar>
#include <QLineEdit>
#include <QList>
#include <QLabel>
#include <QHBoxLayout>
#include "build_output.h"

class EDITOR:public QMainWindow
{
    Q_OBJECT

    private:
        BUILD_OUTPUT *compiler_screen;
        QTextEdit *text_container;
        QTextEdit::ExtraSelection *line_highlight;
        QList<QTextEdit::ExtraSelection> *highlight;
        QLineEdit *search_text;
        QVBoxLayout *layout;
        QWidget *main_widget;
        QMenu *file_menu, *edit_menu, *build_menu;
        QAction *build_action, *build_run_action; //*run_action;
        QToolBar *tool_bar;
        // menu for color choosing
        QMenu *set_color_subMenu;
        QMenu *red_color_subMenu,*pink_color_subMenu,*purple_color_subMenu,*blue_color_subMenu;
        // file menu actions
        QAction *open, *save, *new_file, *exit_action, *close_action;
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

        QStatusBar *status_bar;
        QLabel *current_line, *total_line;
        QLabel *status_msg; // fake spacer
        QHBoxLayout *status_layout;
        std::string file;
        QString title;
        QString file_types;
        size_t is_cpp, is_c;
        char *content = nullptr;
        bool something_changed = false;
        bool create_new = false;
        void create_action();
        void create_color_menu();
        void create_color_action();
        void initialize_file_types();
        void save_with_new_name();
        void save_with_previous_name();
        void set_total_line_num();
        void set_compiler_output(std::string);
        std::string run_command(std::string);
        void closeEvent(QCloseEvent *);

    public:
        EDITOR(QWidget *parent = 0);
        ~EDITOR();

    private slots:
        void open_file();
        void load_file();
        void save_file();
        void close_file();
        void text_changed();
        void new_file_create();
        void exit_triggered();
        void search(QString);
        void set_current_line_num();
        void build_file();
        void build_and_run_file();
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

};

#endif
