#include "build_output.h"

BUILD_OUTPUT::BUILD_OUTPUT(QWidget *parent):QDialog(parent)
{
    main_parent = parent;
    setMinimumHeight(350);
    setMinimumWidth(500);
    setWindowTitle("compiler output");
    output_container = new QTextEdit;
    output_container->setStyleSheet("background-color:#263238; font-weight:bold;");
    layout = new QVBoxLayout;
    layout->addWidget(output_container);
    setLayout(layout);
}

void BUILD_OUTPUT::closeEvent(QCloseEvent *ev)
{
    main_parent->setEnabled(true);
    ev->accept();
}

void BUILD_OUTPUT::clear()
{
    output_container->clear();
}

void BUILD_OUTPUT::set_output(char *output)
{
    output_container->setText(output);
}

void BUILD_OUTPUT::set_output(QString output)
{
    output_container->setText(output);
}

BUILD_OUTPUT::~BUILD_OUTPUT()
{
    delete layout;
    delete output_container;
}
