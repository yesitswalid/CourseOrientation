#ifndef GESTIONPORTIQUE_H
#define GESTIONPORTIQUE_H

#include <ui_gestionportique.h>
#include <QWidget>
#include <QMainWindow>
#include <QtWidgets>

class GestionPortique : public QMainWindow
{
    Q_OBJECT


public:
     GestionPortique();
    ~GestionPortique();

private:
    Ui::GestionPortique ui;
};

#endif // GESTIONPORTIQUE_H
