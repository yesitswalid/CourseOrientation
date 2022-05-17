#ifndef STATISTIQUE_H
#define STATISTIQUE_H


#include <QtWidgets>
#include <ui_statistique.h>

class Statistique : public QMainWindow
{
    Q_OBJECT

public:
    explicit Statistique();
    ~Statistique();
    void init();

private:
    Ui::Statistique ui;
};

#endif // STATISTIQUE_H
