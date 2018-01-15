#include "mainwindow.h"
#include <QApplication>
#include <iostream>
char *ext_file;

int main(int argc, char *argv[])
{
    int rt_vl;
    QApplication a(argc, argv);
    EDITOR *w = new EDITOR;
    if(argc == 2)
    {
       ext_file = new char[strlen(argv[1]) + 1];
       strcpy(ext_file, argv[1]);
       w->load_from_open_with_option(ext_file);
       delete[] ext_file;
    }

    w->show();
    rt_vl = a.exec();
    delete w;
    return rt_vl;
}
