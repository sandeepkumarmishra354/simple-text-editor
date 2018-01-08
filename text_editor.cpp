#include "text_editor.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QColor>
#include <iostream>
#include <fstream>

EDITOR::EDITOR(QWidget *parent):QMainWindow(parent)
{
    text_container = new QTextEdit;
    text_container->setVisible(false);
    connect(text_container, SIGNAL(cursorPositionChanged()), this, SLOT(set_line_num()));
    text_container->setLineWrapMode(QTextEdit::NoWrap); // dispaly a horizontal scrollbar
    text_container->setStyleSheet("background-color:#263238; color: white; font-weight:bold;");
    status_bar = statusBar();
    current_line = new QLabel("current line 0");
    total_line = new QLabel("total line 0");
    status_msg = new QLabel("msg");
    status_layout = new QHBoxLayout;
    //status_layout->addWidget(status_msg);
    //status_layout->addWidget(current_line);
    //status_layout->addWidget(total_line);
    status_bar->addPermanentWidget(current_line);
    //status_bar->addPermanentWidget(spacer_label);
    status_bar->addPermanentWidget(total_line);
    //status_bar->setLayout(status_layout);
    file_menu = menuBar()->addMenu("File");
    tool_bar = addToolBar(tr("toolbar"));
    tool_bar->setMovable(false);
    search_text = new QLineEdit;
    search_text->setToolTip("enter text to search");
    search_text->setEnabled(false);
    search_text->setFixedWidth(100);
    connect(search_text, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));
    create_action();
    edit_menu = menuBar()->addMenu("Edit");
    set_color_subMenu = edit_menu->addMenu("change color");
    set_color_subMenu->setEnabled(false);
    create_color_menu();
    create_color_action();
    initialize_file_types();
    connect(text_container, SIGNAL(textChanged()), this, SLOT(text_changed()));
    build_menu = menuBar()->addMenu("buid_run");
    layout = new QVBoxLayout;
    layout->addWidget(text_container);
    main_widget = new QWidget;
    main_widget->setLayout(layout);
    layout->setContentsMargins(20, 0, 5, 0);
    setMinimumHeight(450);
    setMinimumWidth(700);
    setCentralWidget(main_widget);
    setWindowIcon(QIcon(":/icons/main.ico"));
    setWindowTitle("Editor");
}

void EDITOR::create_action()
{
    // file menu actions
    new_file = new QAction("New", this);
    new_file->setShortcut(QKeySequence::New);
    new_file->setIcon(QIcon(":/icons/file.ico"));
    connect(new_file, SIGNAL(triggered()), this, SLOT(new_file_create()));
    open = new QAction("open", this);
    open->setShortcut(tr("CTRL+O"));
    open->setIcon(QIcon(":/icons/open.png"));
    connect(open, SIGNAL(triggered()), this, SLOT(open_file()));
    save = new QAction("save", this);
    save->setShortcut(tr("CTRL+S"));
    save->setIcon(QIcon(":/icons/save.ico"));
    save->setEnabled(false);
    connect(save, SIGNAL(triggered()), this, SLOT(save_file()));
    close_action = new QAction("save and close", this);
    close_action->setShortcut(tr("CTRL+C"));
    close_action->setIcon(QIcon(":/icons/save_and_close.png"));
    close_action->setEnabled(false);
    connect(close_action, SIGNAL(triggered()), this, SLOT(close_file()));
    exit_action = new QAction("exit", this);
    exit_action->setShortcut(QKeySequence::Close);
    exit_action->setIcon(QIcon(":/icons/exit.png"));
    connect(exit_action, SIGNAL(triggered()), this, SLOT(exit_triggered()));
    file_menu->addAction(new_file);
    file_menu->addAction(open);
    file_menu->addAction(save);
    file_menu->addAction(close_action);
    file_menu->addAction(exit_action);

    // tool bar actions
    tool_bar->addAction(new_file);
    tool_bar->addAction(open);
    tool_bar->addAction(save);
    tool_bar->addAction(close_action);
    tool_bar->addAction(exit_action);
    tool_bar->addSeparator();
    tool_bar->addWidget(search_text);

}

void EDITOR::create_color_menu()
{
    red_color_subMenu = set_color_subMenu->addMenu("red");
    pink_color_subMenu = set_color_subMenu->addMenu("pink");
    purple_color_subMenu = set_color_subMenu->addMenu("purple");
    blue_color_subMenu = set_color_subMenu->addMenu("blue");
}

void EDITOR::create_color_action()
{
    // red color action
    color_ffcdd2 = new QAction("ffcdd2", this);
    connect(color_ffcdd2, SIGNAL(triggered()), this, SLOT(set_color_ffcdd2()));
    color_ef9a9a = new QAction("ef9a9a", this);
    connect(color_ef9a9a, SIGNAL(triggered()), this, SLOT(set_color_ef9a9a()));
    color_e57373 = new QAction("e57373", this);
    connect(color_e57373, SIGNAL(triggered()), this, SLOT(set_color_e57373()));
    red_color_subMenu->addAction(color_ffcdd2);
    red_color_subMenu->addAction(color_ef9a9a);
    red_color_subMenu->addAction(color_e57373);

    // pink color action
    color_e91e63 = new QAction("e91e63", this);
    connect(color_e91e63, SIGNAL(triggered()), this, SLOT(set_color_e91e63()));
    color_c2185b = new QAction("c2185b", this);
    connect(color_c2185b, SIGNAL(triggered()), this, SLOT(set_color_c2185b()));
    color_c51162 = new QAction("c51162", this);
    connect(color_c51162, SIGNAL(triggered()), this, SLOT(set_color_c51162()));
    pink_color_subMenu->addAction(color_e91e63);
    pink_color_subMenu->addAction(color_c2185b);
    pink_color_subMenu->addAction(color_c51162);

    // purple color action
    color_ce93d8 = new QAction("ce93d8", this);
    connect(color_ce93d8, SIGNAL(triggered()), this, SLOT(set_color_ce93d8()));
    color_9c27b0 = new QAction("9c27b0", this);
    connect(color_9c27b0, SIGNAL(triggered()), this, SLOT(set_color_9c27b0()));
    color_aa00ff = new QAction("aa00ff", this);
    connect(color_aa00ff, SIGNAL(triggered()), this, SLOT(set_color_aa00ff()));
    purple_color_subMenu->addAction(color_ce93d8);
    purple_color_subMenu->addAction(color_9c27b0);
    purple_color_subMenu->addAction(color_aa00ff);

    // blue color action
    color_7986cb = new QAction("7986cb", this);
    connect(color_7986cb, SIGNAL(triggered()), this, SLOT(set_color_7986cb()));
    color_3f51b5 = new QAction("3f51b5", this);
    connect(color_3f51b5, SIGNAL(triggered()), this, SLOT(set_color_3f51b5()));
    color_1a237e = new QAction("1a237e", this);
    connect(color_1a237e, SIGNAL(triggered()), this, SLOT(set_color_1a237e()));
    blue_color_subMenu->addAction(color_7986cb);
    blue_color_subMenu->addAction(color_3f51b5);
    blue_color_subMenu->addAction(color_1a237e);

    // default color action
    default_color = new QAction("set default", this);
    default_color->setEnabled(false);
    connect(default_color, SIGNAL(triggered()), this, SLOT(set_default_color()));
    edit_menu->addAction(default_color);
}

void EDITOR::search(QString txt)
{
    QList<QTextEdit::ExtraSelection> extra_sel;
    text_container->moveCursor(QTextCursor::Start);
    QColor color = QColor(Qt::blue).light(130);
    while(text_container->find(txt))
    {
        QTextEdit::ExtraSelection extra;
        extra.format.setBackground(color);
        extra.cursor = text_container->textCursor();
        extra_sel.append(extra);
    }
    text_container->setExtraSelections(extra_sel);
}

void EDITOR::set_line_num()
{
    line_highlight = new QTextEdit::ExtraSelection;
    line_highlight->cursor = text_container->textCursor();
    line_highlight->format.setProperty(QTextFormat::FullWidthSelection, true);
    QColor color = QColor(Qt::blue).light(120);
    line_highlight->format.setBackground(color);
    highlight = new QList<QTextEdit::ExtraSelection>;
    *highlight << *line_highlight;
    text_container->setExtraSelections(*highlight);
    QString line_num = "current line ";
    int ln = text_container->textCursor().blockNumber() + 1;
    line_num += QString::number(ln);
    current_line->setText(line_num);
}

// defining red color slots
void EDITOR::set_color_ffcdd2()
{
    text_container->setStyleSheet("background-color:#ffcdd2");
}
void EDITOR::set_color_ef9a9a()
{
    text_container->setStyleSheet("background-color:#ef9a9a");
}
void EDITOR::set_color_e57373()
{
    text_container->setStyleSheet("background-color:#e57373");
}
// defining pink color slots
void EDITOR::set_color_e91e63()
{
    text_container->setStyleSheet("background-color:#e91e63");
}
void EDITOR::set_color_c2185b()
{
    text_container->setStyleSheet("background-color:#c2185b");
}
void EDITOR::set_color_c51162()
{
    text_container->setStyleSheet("background-color:pink");
}
// defining purple color slots
void EDITOR::set_color_ce93d8()
{
    text_container->setStyleSheet("background-color:#ce93d8");
}
void EDITOR::set_color_9c27b0()
{
    text_container->setStyleSheet("background-color:#9c27b0");
}
void EDITOR::set_color_aa00ff()
{
    text_container->setStyleSheet("background-color:#aa00ff");
}
// defining blue color slots
void EDITOR::set_color_7986cb()
{
    text_container->setStyleSheet("background-color:#7986cb");
}
void EDITOR::set_color_3f51b5()
{
    text_container->setStyleSheet("background-color:#3f51b5");
}
void EDITOR::set_color_1a237e()
{
    text_container->setStyleSheet("background-color:#1a237e");
}
// defining default color slot
void EDITOR::set_default_color()
{
    text_container->setStyleSheet("background-color:#263238; color: white; font-weight:bold;");
}

void EDITOR::save_file()
{
    if(something_changed)
    {
        if(create_new)
        {
            QString new_file_path;
            new_file_path = QFileDialog::getSaveFileName(this, "save file", ".", file_types);
            if(!new_file_path.isEmpty())
            {
                file = new_file_path.toStdString();
                std::ofstream file_save(file);
                QString new_text = text_container->toPlainText();
                std::string text_to_save = new_text.toStdString();
                file_save<<text_to_save;
                file_save.close();
                something_changed = false;
                create_new = false;
                title = new_file_path;
                setWindowTitle(title);
                status_bar->showMessage("saved", 4000);
            }
        }
        else
        {
            std::ifstream for_check(file);
            if(for_check.is_open())
            {
                for_check.close();
                std::ofstream file_save(file, std::ios::trunc);
                if(file_save.is_open())
                {
                    QString new_text = text_container->toPlainText();
                    std::string text_to_save = new_text.toStdString();
                    file_save<<text_to_save;
                    file_save.close();
                    something_changed = false;
                    setWindowTitle(title);
                    status_bar->showMessage("saved", 4000);
                }
            }
            else
            {
                int op;
                op = QMessageBox::warning(this, "EROOR", "source file may be deleted"
                     " or some other error occurd\nText_Editor unable to read the source file"
                     "\n*****save as previous or create new file ? *****",
                     QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

                switch(op)
                {
                    case QMessageBox::Yes:
                        save_with_previous_name();
                        break;
                    case QMessageBox::No:
                        save_with_new_name();
                        break;
                    case QMessageBox::Cancel:
                        break;
                }
            }
        }
    }
}

void EDITOR::text_changed()
{
    something_changed = true;
    close_action->setEnabled(true);
    if(!save->isEnabled())
        save->setEnabled(true);
    setWindowTitle("*" + title);
}

void EDITOR::initialize_file_types()
{
    file_types = "text type("
                 "*.txt *.TXT *.c *.C *.cpp *.CPP *.py "
                 "*.PY *.java *.JAVA *.s *.asm *.cc *.conf "
                 "*.config *.json *.JSON *.html *.htm *.HTML *.HTM"
                 "*.js *.JS *.css *.CSS *.h *.H *.hpp *.HPP *.xml *.XML"
                 "*.pro *.user *.md *.MD *.key *.KEY *.bat *.BAT *.sh *.SH"
                 "*.properties *.iml *.gradle *.cert *.vim *.el *.srt *.sub"
                 "*.rst"
                 ")";
}

void EDITOR::closeEvent(QCloseEvent *event)
{
    if(something_changed || create_new)
        exit_triggered();
    else
        event->accept();
}

void EDITOR::save_with_new_name()
{
    QString new_file_path;
    new_file_path = QFileDialog::getSaveFileName(this, "save file", ".", file_types);
    file = new_file_path.toStdString();
    std::ofstream file_save(file);
    QString new_text = text_container->toPlainText();
    std::string text_to_save = new_text.toStdString();
    file_save<<text_to_save;
    file_save.close();
    something_changed = false;
    create_new = false;
    title = new_file_path;
    setWindowTitle(title);
    status_bar->showMessage("saved", 4000);
}

void EDITOR::save_with_previous_name()
{
    std::ofstream file_save(file);
    if(file_save.is_open())
    {
        QString new_text = text_container->toPlainText();
        std::string text_to_save = new_text.toStdString();
        file_save<<text_to_save;
        file_save.close();
        something_changed = false;
        setWindowTitle(title);
        status_bar->showMessage("saved", 4000);
    }
}

void EDITOR::exit_triggered()
{
    if(something_changed)
    {
        int op;
        op = QMessageBox::warning(this, "Are You Sure ?", "there is unsaved details\nSave and exit ??",
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        switch(op)
        {
            case QMessageBox::Yes:
                save_file();
                close();
                break;
            case QMessageBox::No:
                close();
                break;
            case QMessageBox::Cancel:
                break;
        }
    }
    else
        close();
}

void EDITOR::close_file()
{
    if(something_changed)
        save_file();

    text_container->setVisible(false);
    close_action->setEnabled(false);
    save->setEnabled(false);
    search_text->setEnabled(false);
    set_color_subMenu->setEnabled(false);
    default_color->setEnabled(false);
    something_changed = false;
    create_new = false;
    setWindowTitle("Editor");
}

void EDITOR::open_file()
{
    QString cur_file;
    cur_file = QFileDialog::getOpenFileName(this, "open .txt file only",".",file_types);
    if(!cur_file.isEmpty())
    {
        file = cur_file.toStdString();
        title = cur_file;
        load_file();
    }
}

void EDITOR::load_file()
{
    int beg, end;
    int content_size;
    int i = 0;
    std::ifstream text_file(file);
    beg = text_file.tellg();
    text_file.seekg(0, std::ios::end);
    end = text_file.tellg();
    text_file.seekg(0, std::ios::beg);
    content_size = end - beg;
    content = new char[content_size + 1];
    text_file.seekg(0, std::ios::beg);
    while (text_file)
        content[i++] = text_file.get();
    content[--i] = '\0';
    text_file.close();
    text_container->setText(content);
    setWindowTitle(title);
    status_bar->showMessage("loaded", 4000);
    something_changed = false;
    create_new = false;
    text_container->setVisible(true);
    close_action->setEnabled(true);
    delete[] content;
    save->setEnabled(true);
    search_text->setEnabled(true);
    set_color_subMenu->setEnabled(true);
    default_color->setEnabled(true);
    content = nullptr;
}

void EDITOR::new_file_create()
{
    if(!something_changed)
    {
        setWindowTitle("untitled");
        something_changed = false;
        title = "untitled";
        text_container->setVisible(true);
        search_text->setEnabled(true);
        set_color_subMenu->setEnabled(true);
        default_color->setEnabled(true);
        create_new = true;
    }
}

EDITOR::~EDITOR()
{
    delete text_container;
    delete search_text;
    delete main_widget;
    delete layout;
    delete file_menu;
    delete edit_menu;
    delete build_menu;
    delete open;
    delete save;
    delete close_action;
    delete set_color_subMenu;
    delete red_color_subMenu;
    delete pink_color_subMenu;
    delete purple_color_subMenu;
    delete blue_color_subMenu;

    // red
    delete color_ffcdd2;
    delete color_ef9a9a;
    delete color_e57373;
    // pink
    delete color_e91e63;
    delete color_c2185b;
    delete color_c51162;
    // purple
    delete color_ce93d8;
    delete color_9c27b0;
    delete color_aa00ff;
    // blue
    delete color_7986cb;
    delete color_3f51b5;
    delete color_1a237e;

    //delete fmt;
    delete default_color;
    delete new_file;
    delete status_bar;
    delete tool_bar;
    delete highlight;
    delete line_highlight;
    delete total_line;
    delete current_line;
    delete status_msg;
    delete status_layout;
    delete this;
}

