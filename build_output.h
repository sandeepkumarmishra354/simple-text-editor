#ifndef BUILD_OUTPUT_H
#define BUILD_OUTPUT_H
#include <QDialog>
#include <QWidget>
#include <QCloseEvent>
#include <QTextEdit>
#include <QVBoxLayout>

class BUILD_OUTPUT:public QDialog
{
    Q_OBJECT

    private:
        QTextEdit *output_container;
        QVBoxLayout *layout;
        QWidget *main_parent = nullptr;
        void closeEvent(QCloseEvent*);

    public:
        BUILD_OUTPUT(QWidget *parent = 0);
        ~BUILD_OUTPUT();
        void set_output(char *);
        void set_output(QString);
        void clear();
};

#endif // BUILD_OUTPUT_H
