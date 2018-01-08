#include "text_editor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    EDITOR *w = new EDITOR;
    w->show();
    return a.exec();
}
