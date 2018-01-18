#include "mainwindow.h"
#include <QMenuBar>
#include <QFileDialog>
#include <QMessageBox>
#include <QCloseEvent>
#include <QTextCursor>
#include <QScrollBar>
#include <QColor>
#include <QApplication>
#include <iostream>
#include <fstream>
#include <unistd.h>

EDITOR::EDITOR(QWidget *parent):QMainWindow(parent)
{
    signal(SIGINT, sig_handle); // prevent program termination from ctrl+c

    text_container = new QTextEdit;
    text_container->setVisible(false);
    connect(text_container, SIGNAL(cursorPositionChanged()), this, SLOT(set_current_line_num()));
    connect(text_container, SIGNAL(textChanged()), this, SLOT(text_changed()));
    text_container->setLineWrapMode(QTextEdit::NoWrap); // dispaly a horizontal scrollbar
    text_container->setStyleSheet("background-color:#263238; color: white; font-weight:bold;");

    status_bar = statusBar();

    current_line = new QLabel("current line 0");
    current_line->setStyleSheet("font-weight:bold; color: red");
    current_line->setVisible(false);
    total_line = new QLabel("total line 0");
    total_line->setStyleSheet("font-weight:bold; color: blue");
    total_line->setVisible(false);

    status_layout = new QHBoxLayout;

    status_bar->addPermanentWidget(current_line);
    status_bar->addPermanentWidget(total_line);

    tool_bar = addToolBar(tr("toolbar"));
    tool_bar->setMovable(false);
    tool_bar->setFixedHeight(65);

    search_text = new QLineEdit;
    search_text->setToolTip("enter text to search");
    search_text->setEnabled(false);
    search_text->setFixedWidth(100);
    connect(search_text, SIGNAL(textChanged(QString)), this, SLOT(search(QString)));

    file_menu = menuBar()->addMenu(tr("&File"));
    edit_menu = menuBar()->addMenu(tr("&Edit"));
    build_menu = menuBar()->addMenu(tr("&Buid_Run"));
    about_menu = menuBar()->addMenu(tr("&Help"));
    menuBar()->setStyleSheet("border:none");
    build_menu->setToolTip("only c/c++");
    create_action();

    set_color_subMenu = edit_menu->addMenu("change color");
    set_color_subMenu->setEnabled(false);
    create_color_menu();
    create_color_action();

    initialize_file_types();

    layout = new QVBoxLayout;
    layout->addWidget(text_container);
    main_widget = new QWidget;
    main_widget->setLayout(layout);
    layout->setContentsMargins(10, 0, 5, 0);
    setMinimumHeight(500);
    setMinimumWidth(750);

    setCentralWidget(main_widget);
    setWindowIcon(QIcon(":/icons/main.ico"));
    setWindowTitle("Editor");
    showMaximized();
    is_c = is_cpp = DO_NOT_COMPILE;
    is_py = DO_NOT_INTERPRET;
    new_file_create();
}

void sig_handle(int sg)
{
    if(sg == SIGINT)
        std::cout<<"ctrl+c";
}

void EDITOR::create_action()
{
    /***** new_file action *****/
    new_file = new QAction("New", this);
    new_file->setShortcut(QKeySequence::New);
    new_file->setIcon(QIcon(":/icons/file.ico"));
    connect(new_file, SIGNAL(triggered()), this, SLOT(new_file_create()));

    /***** open action *****/
    open = new QAction("open", this);
    open->setShortcut(tr("CTRL+O"));
    open->setIcon(QIcon(":/icons/open.png"));
    connect(open, SIGNAL(triggered()), this, SLOT(open_file()));

    /***** save action *****/
    save = new QAction("save", this);
    save->setShortcut(tr("CTRL+S"));
    save->setIcon(QIcon(":/icons/save.ico"));
    save->setEnabled(false);
    connect(save, SIGNAL(triggered()), this, SLOT(save_file()));

    /***** close action *****/
    close_action = new QAction("save and close", this);
    close_action->setShortcut(tr("CTRL+C"));
    close_action->setIcon(QIcon(":/icons/save_and_close.png"));
    close_action->setEnabled(false);
    connect(close_action, SIGNAL(triggered()), this, SLOT(close_file()));

    /***** quit action *****/
    quit_action = new QAction("Quit", this);
    quit_action->setShortcut(QKeySequence::Close);
    quit_action->setIcon(QIcon(":/icons/exit.png"));
    connect(quit_action, SIGNAL(triggered()), this, SLOT(quit_triggered()));

    file_menu->addAction(new_file);
    file_menu->addAction(open);
    file_menu->addAction(save);
    file_menu->addAction(close_action);
    file_menu->addAction(quit_action);

    /***** build menu action *****/
    build_run_action = new QAction("build/run");
    build_run_action->setIcon(QIcon(":/icons/build_run.png"));
    connect(build_run_action, SIGNAL(triggered()), this, SLOT(build_and_run_file()));
    build_run_action->setEnabled(false);

    build_menu->addAction(build_run_action);

    /***** about menu action *****/
    about_qt = new QAction("about Qt");
    connect(about_qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    about_this = new QAction("about");
    connect(about_this, SIGNAL(triggered()), this, SLOT(about_us()));

    about_menu->addAction(about_qt);
    about_menu->addAction(about_this);

    /***** toolbar action *****/
    tool_bar->addAction(new_file);
    tool_bar->addAction(open);
    tool_bar->addAction(save);
    tool_bar->addAction(close_action);
    tool_bar->addAction(quit_action);
    tool_bar->addSeparator();
    tool_bar->addAction(build_run_action);
    tool_bar->addSeparator();
    tool_bar->addWidget(search_text);
    tool_bar->addSeparator();

    /***** music action *****/
    music_action = new QAction("Play Music");
    music_action->setIcon(QIcon(":/icons/music.png"));
    connect(music_action, SIGNAL(triggered()), this, SLOT(play_music()));
    tool_bar->addAction(music_action);

    /***** music stop action *****/
    music_stop = new QAction("Shutdown Music");
    music_stop->setIcon(QIcon(":/icons/music_stop.png"));
    connect(music_stop, SIGNAL(triggered()), this, SLOT(stop_music()));
    music_stop->setVisible(false);
    tool_bar->addAction(music_stop);
    tool_bar->addSeparator();

    /***** play pause action *****/
    play_pause_action = new QAction("play/pause");
    connect(play_pause_action, SIGNAL(triggered()), this, SLOT(play_pause()));
    play_pause_action->setIcon(QIcon(":/icons/pause.png"));
    play_pause_action->setVisible(false);

    /***** next previous action *****/
    next = new QAction("Next Track");
    next->setIcon(QIcon(":/icons/next.png"));
    connect(next, SIGNAL(triggered()), this, SLOT(play_next()));
    next->setVisible(false);
    prev = new QAction("Previous Track");
    connect(prev, SIGNAL(triggered()), this, SLOT(play_prev()));
    prev->setIcon(QIcon(":/icons/previous.png"));
    prev->setVisible(false);

    tool_bar->addAction(prev);
    tool_bar->addAction(play_pause_action);
    tool_bar->addAction(next);
}

void EDITOR::play_pause()
{
    // checks if m_player is already initialised or not
    if(m_player != NOT_INITIALISED)
    {
        if(!ply) // means pause icon is showing
        {
            play_pause_action->setIcon(QIcon(":/icons/play.png"));
            ply = true;
        }
        else
        {
            play_pause_action->setIcon(QIcon(":/icons/pause.png"));
            ply = false;
        }

        m_player->play_or_pause(); // this function exists in header file "music.h"
    }
}

void EDITOR::play_music()
{
    // collects all selected files
    QStringList playlist = QFileDialog::getOpenFileNames(this, "select one or multiple files",
                                                         "/root/Music", "*.mp3");
    if(!playlist.isEmpty())
    {
        if(m_player == NOT_INITIALISED) // this means m_player is not initialised before
        {
            // enable all control buttons
            next->setVisible(true);
            prev->setVisible(true);
            play_pause_action->setVisible(true);
            music_stop->setVisible(true);
            // initialise m_player
            m_player = new MUSIC(this);
            // get_layout function returns main widget container
            tool_bar->addWidget(m_player->get_layout());
            // play the selected files
            m_player->play_this(playlist);
        }
        else
            m_player->play_this(playlist);
    }
}

void EDITOR::stop_music()
{
    if(m_player != NOT_INITIALISED)
    {
        delete m_player;
        m_player = NOT_INITIALISED;
        next->setVisible(false);
        prev->setVisible(false);
        play_pause_action->setVisible(false);
        music_stop->setVisible(false);
    }
}

void EDITOR::play_next()
{
    m_player->play_next();
}

void EDITOR::play_prev()
{
    m_player->play_prev();
}

void EDITOR::about_us()
{
    QMessageBox::about(this, "About", "<b>About</b> <br><br>"
                                      "This is a simple text-editor with some compiler functionality.\n"
                                      "Supported programming language- <font color=\"blue\">C/C++,PYTHON</font>"
                                      "<br><br> this is a open source project. All source files are available at "
                                      "<a href=\"https://github.com/sandeepkumarmishra354/simple-text-editor\">"
                                      "https://github.com/sandeepkumarmishra354/simple-text-editor\</a>"
                                      "<br><br>Email Me- <font color=\"blue\">sandeepkumarmishra354@gmail.com</font>"
                                      "<br><br><br><br><b>Thank You</b>");
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

void EDITOR::compiler(std::string cmd, std::string exact_file_name)
{
    // insert ./ to executable ex- ./exec
    exact_file_name.insert(0, "./");
    // run command and returns command output as std::string
    std::string cmd_output = run_command(cmd);
    size_t err = cmd_output.find("error:");

    if(err == std::string::npos)// means no error found
    {
        // open new shell command
        std::string shell_open_cmd = "gnome-terminal --window-with-profile=Bash -- bash -c ";
        // run and pause after execution finished
        std::string run_and_pause = "\""+ exact_file_name + ";printf '\npress enter to exit';read; exec shell \" ";
        // concat both commands
        std::string run_this = shell_open_cmd + run_and_pause;
        // run the command
        system(run_this.c_str());
    }
    else// means some error
    {
        display_error(cmd_output);// display the error message
    }
}

void EDITOR::interpreter(std::string cmd)
{
    std::string shell_open_cmd = "gnome-terminal --window-with-profile=Bash -- bash -c ";
    std::string run_and_pause = "\""+ cmd + ";printf '\npress enter to exit';read; exec shell \" ";
    std::string run_this = shell_open_cmd + run_and_pause;
    system(run_this.c_str());
}

std::string EDITOR::run_command(std::string cmd)
{
    std::string data;
    FILE * stream;
    const int max_buffer = 256;
    char buffer[max_buffer];
    cmd.append(" 2>&1");

    // this function runs the command
    // and save the output in temp file
    // and return a file descripter to that temp file
    stream = popen(cmd.c_str(), "r");
    if (stream)
    {
        while (!feof(stream)) // since last position of stream
            // read all bytes in buffer
            if (fgets(buffer, max_buffer, stream) != NULL)
                data.append(buffer);
        // close file
        pclose(stream);
    }

   return data;
}

void EDITOR::display_error(std::string err)
{
    // length of error message
    size_t len = err.length();
    if(len > 300)
    {
        std::string first_half;
        first_half = err.substr(0,300);
        QString err_msg = QString::fromStdString(first_half);
        QMessageBox ERROR(QMessageBox::Critical,"ERROR",err_msg,QMessageBox::Close);
        // add option like show details for more info.
        ERROR.setDetailedText(QString::fromStdString(err));
        ERROR.exec();
    }
    else
    {
        QMessageBox ERROR(QMessageBox::Critical,"ERROR",QString::fromStdString(err),QMessageBox::Close);
        ERROR.exec();
    }
}

// this function invokes when someone choose the option open with
// and selects this program for opening file
void EDITOR::load_from_open_with_option(char *file_to_load)
{
    // return std::string equal to char[]
    file = std::string(file_to_load,strlen(file_to_load));
    size_t end_fld = file.rfind("/");
    std::string f_path = file.substr(0, end_fld);
    chdir(f_path.c_str());
    // check that file is c/c++, python or not
    check_file_extension(QString::fromStdString(file));
    // return QString equal to std::string
    title = QString::fromStdString(file);
    // now load the file
    load_file();
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

void EDITOR::build_and_run_file()
{
    std::string compiler_output;
    std::string this_file;
    size_t end_of_folder_path = file.rfind("/");
    std::string this_dir = file.substr(0, end_of_folder_path);
    std::string file_name = file.substr(end_of_folder_path+1);
    size_t last_dot = file_name.find(".");
    std::string exact_file_name = file_name.substr(0, last_dot);

    if(is_c) // for c file
        this_file = "gcc " + file_name + " -o " + exact_file_name;
    if(is_cpp) // for cpp file
        this_file = "g++ " + file_name + " -o " + exact_file_name;
    if(is_py) // for python file
        this_file = "python " + file_name;

    if(is_c || is_cpp)
    {
        COMPILE(this_file, exact_file_name);
    }
    if(is_py)
    {
        INTERPRET(this_file);
    }
}

void EDITOR::set_current_line_num()
{
    line_highlight = new QTextEdit::ExtraSelection;
    line_highlight->cursor = text_container->textCursor();
    line_highlight->format.setProperty(QTextFormat::FullWidthSelection, true);
    QColor color = QColor(Qt::black).light(160);
    line_highlight->format.setBackground(color);
    highlight = new QList<QTextEdit::ExtraSelection>;
    *highlight << *line_highlight;
    text_container->setExtraSelections(*highlight);
    QString line_num = "current line ";
    int ln = text_container->textCursor().blockNumber() + 1;
    line_num += QString::number(ln);
    current_line->setText(line_num);
}

void EDITOR::set_total_line_num()
{
    QString line_txt = "total line ";
    QTextCursor cur_pos = text_container->textCursor();
    text_container->moveCursor(QTextCursor::End);
    int line_num = text_container->textCursor().blockNumber() + 1;
    text_container->setTextCursor(cur_pos);
    line_txt += QString::number(line_num);
    total_line->setText(line_txt);
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
    if(something_changed)// means some unsaved details
    {
        if(create_new)// new file created
        {
            QString new_file_path;
            // return full file path
            new_file_path = QFileDialog::getSaveFileName(this, "save file", ".", file_types);
            if(!new_file_path.isEmpty())
            {
                file = new_file_path.toStdString();
                // open file for writing content in it
                std::ofstream file_save(file);
                // return the whole text
                QString new_text = text_container->toPlainText();
                std::string text_to_save = new_text.toStdString();
                // write text in file
                file_save<<text_to_save;
                file_save.close();
                // now no unsaved details and no new file created at this time
                something_changed = false;
                create_new = false;
                title = new_file_path;
                setWindowTitle(title);
                status_bar->showMessage("saved", 4000);
                // change the current working directory
                chdir(file.c_str());
                check_file_extension(new_file_path);

            }
        }
        else // no new file created
        {
            // open file to check that file is exist or not
            std::ifstream for_check(file);
            if(for_check.is_open()) // means file exists
            {
                for_check.close();// then close the file
                // open file and update its contents
                std::ofstream file_save(file, std::ios::trunc);
                if(file_save.is_open())
                {
                    QString new_text = text_container->toPlainText();
                    std::string text_to_save = new_text.toStdString();
                    file_save<<text_to_save;
                    file_save.close();
                    // now no unsaved details
                    something_changed = false;
                    setWindowTitle(title);
                    status_bar->showMessage("saved", 4000);
                }
            }
            else// this means file must be deleted or some other error occured
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

void EDITOR::check_file_extension(QString check)
{
    file = check.toStdString();
    is_cpp = file.rfind(".cpp");
    if(is_cpp == std::string::npos)
    {
        is_cpp = file.rfind(".CPP");
        if(is_cpp == std::string::npos)
            is_cpp = DO_NOT_COMPILE;
    }
    if(is_cpp == DO_NOT_COMPILE)
    {
        is_c = file.rfind(".c");
        if(is_c == std::string::npos)
        {
            is_c = file.rfind(".C");
            if(is_c == std::string::npos)
                is_c = DO_NOT_COMPILE;
        }
    }
    if(is_cpp == DO_NOT_COMPILE && is_c == DO_NOT_COMPILE)
    {
        is_py = file.rfind(".py");
        if(is_py == std::string::npos)
        {
            is_py = file.rfind(".PY");
            if(is_py == std::string::npos)
                is_py = DO_NOT_INTERPRET;
        }
    }
    if(is_c || is_cpp || is_py)
    {
        build_run_action->setEnabled(true);
    }
    else
    {
        build_run_action->setEnabled(false);
    }
}

void EDITOR::text_changed()
{
    set_total_line_num();
    something_changed = true;
    close_action->setEnabled(true);
    if(!save->isEnabled())
        save->setEnabled(true);
    setWindowTitle("**" + title);
}

void EDITOR::initialize_file_types()
{
    // these files are supported by this program
    // it covers almost all text type files

    file_types = "text type("
                 "*.txt *.TXT *.c *.C *.cpp *.CPP *.py "
                 "*.PY *.java *.JAVA *.s *.asm *.cc *.conf "
                 "*.config *.json *.JSON *.html *.htm *.HTML *.HTM"
                 "*.js *.JS *.css *.CSS *.h *.H *.hpp *.HPP *.xml *.XML"
                 "*.pro *.user *.md *.MD *.key *.KEY *.bat *.BAT *.sh *.SH"
                 "*.properties *.iml *.gradle *.cert *.vim *.el *.srt *.sub"
                 "*.rst *.info *.desktop *.profile *.viminfo *.xbel *.cache"
                 "*.home *.lock *.log *.ini *.man *.MAN *.tmp *.TMP *.prop *.ui *.1st *.1ST"
                 "*.abw *.ABW *.act *.ACT *.adoc *.ADOC *.doc *.DOC *.docx *.DOCX *.DOX"
                 "*.dsc *.DSC *.eml *.EML *.emlx *.EMLX *.err *.ERR *.faq *.FAQ *.FDF"
                 "*.fdx *.FDX *.fdxt *.FDXT *.fft *.FFT *gmd *.GMD *.gthr *.GTHR *.hs *.HS"
                 "*.idx *.IDX *.ipf *.IPF *.KLG *.klg *.kon *.KON *.lst *.LST *.ltr *.LTR"
                 "*.lue *.LUE *.mbox *.MBOX *.md5 *.MD5 *md5.txt *.MD5.TXT *.me *.ME *.msg *.MSG"
                 "*.now *.NOW *.odif *.ODIF *.ort *.ORT *.pages *.PAGES *.pages-tef *.PAGES-TEF"
                 "*.plain *.PLAIN *.prt *.PRT *.pvm *.PVM *.readme *.README *.rft *.RFT *.rpt *.RPT"
                 "*.rtf *.RTF *.rzk *.RZK *.save *.SAVE *.session *.SESSION *.sig *.SIG *.sms *.SMS"
                 "*.unx *.UNX *.uof *.UOF *.uot *.UOT *.upd *.UPD *.utf8 *.UTF8 *.utxt *.UTXT *.vct"
                 "*.VCT *.vnt *.VNT *.wtx *.WTX *.xdl *.XDL *.zw *.ZW *.com *.COM *.asp *.ASP *.aspx"
                 "*.ASPX *.jsp *.JSP *.php *.PHP *.rss *.RSS *.xhtml *.XHTML *.class *.CLASS *.cs *.CS"
                 "*.swift *.SWIFT *.vb *.VB *.cfg *.CFG *.dmp *.icns *.drv *.ini *.sys *.csv *.CSV"
                 ")";
}

// this function invokes when someone
// clicks X button or when close() event generated
void EDITOR::closeEvent(QCloseEvent *event)
{
    // if there is some unsaved details
    // first save the detail and then exit
    if(something_changed)
        quit_triggered();

    else// no unsaved details
    {
        if(m_player != NOT_INITIALISED)
        {
            delete m_player;
            m_player = NOT_INITIALISED;
        }
        event->accept();
    }
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

void EDITOR::quit_triggered()
{
    if(something_changed)// there is unsaved details
    {
        int op;
        // show a warning message
        op = QMessageBox::warning(this, "Are You Sure ?",
                                  "there is unsaved details\ndo you want to save it before closing ?",
                                   QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        switch(op)
        {
            case QMessageBox::Save:
                    save_file();
                    close();
                    break;
            case QMessageBox::Discard:
                    close();
                    break;
            case QMessageBox::Cancel:
                    break;
        }
    }
    else
    {
        close();
    }
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
    total_line->setText("total line 0");
    current_line->setText("current line 0");
    total_line->setVisible(false);
    current_line->setVisible(false);
    build_run_action->setEnabled(false);

    is_c = is_cpp = DO_NOT_COMPILE;
    is_py = DO_NOT_INTERPRET;
}

void EDITOR::open_file()
{
    QString cur_file;
    cur_file = QFileDialog::getOpenFileName(this, "open .txt file only",".",file_types);
    if(!cur_file.isEmpty())
    {
        file = cur_file.toStdString();
        size_t end_fld = file.rfind("/");
        std::string f_path = file.substr(0, end_fld);
        chdir(f_path.c_str());
        check_file_extension(cur_file);
        title = cur_file;
        // now load the selected file
        load_file();
    }
}

void EDITOR::load_file()
{
    int beg, end;
    int content_size;
    int i = 0;
    // open file for loading
    std::ifstream text_file(file);
    beg = text_file.tellg();// gives the current file position
    text_file.seekg(0, std::ios::end);// set file position at the end of file
    end = text_file.tellg();
    content_size = end - beg;// gives total bytes in file
    content = new char[content_size + 1];
    text_file.seekg(0, std::ios::beg);// set file position at begining of file

    // copy the contents of file into 'content' variable
    while (text_file)
        content[i++] = text_file.get();

    content[--i] = '\0';
    text_file.close();
    // set content to display
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
    total_line->setVisible(true);
    current_line->setVisible(true);
    content = nullptr;
}

void EDITOR::new_file_create()
{
    if(!something_changed)// means no unsaved details
    {
        // clear the previous contents
        text_container->clear();
        setWindowTitle("untitled");
        something_changed = false;
        title = "untitled";
        save->setEnabled(false);
        build_run_action->setEnabled(false);
        text_container->setVisible(true);
        search_text->setEnabled(true);
        set_color_subMenu->setEnabled(true);
        default_color->setEnabled(true);
        total_line->setVisible(true);
        current_line->setVisible(true);
        create_new = true;
        text_container->setFocus();
    }
    else// some unsaved details
        save_file();
}

EDITOR::~EDITOR()
{
    delete main_widget;
    delete tool_bar;
    delete file_menu;
    delete open;
    delete save;
    delete close_action;
    delete quit_action;
    delete new_file;
    delete color_1a237e;
    delete color_3f51b5;
    delete color_9c27b0;
    delete color_7986cb;
    delete color_aa00ff;
    delete color_c2185b;
    delete color_c51162;
    delete color_ce93d8;
    delete color_e91e63;
    delete color_e57373;
    delete color_ef9a9a;
    delete color_ffcdd2;
    delete default_color;
    delete build_run_action;
    delete build_menu;
    delete set_color_subMenu;
    delete status_bar;
    delete status_layout;
    delete about_menu;
    delete about_qt;
    delete about_this;
    delete music_action;
    delete music_stop;

    if(m_player != NOT_INITIALISED)
    {
        delete m_player;
    }
}

