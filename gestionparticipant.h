#ifndef GESTIONPARTICIPANT_H
#define GESTIONPARTICIPANT_H

#include <QSqlError>
#include <QMainWindow>
#include <QtSql>
#include <QtWidgets>
#include <QByteArray>
#include <ui_gestionparticipant.h>
#include "databasemanager.h"
#include <QSerialPort>
#include <configuration.cpp>

class GestionParticipant : public QMainWindow
{
    Q_OBJECT

public:
     GestionParticipant();
     GestionParticipant(DatabaseManager *db);
    ~GestionParticipant();
    void createTableView();
    void init();

private slots:
    void on_suprButton_clicked();

    void on_updateButton_clicked();

    void on_razButton_clicked();

    void razGetData(QByteArray);

    void finishedRead();

    void serialReceived();

   // void on_pushButton_clicked();

    void on_razSimButton_clicked();

    void on_dataSimButton_clicked();

private:
    Ui::GestionParticipant ui;
    void showError(const QSqlError &err);
    //QSqlRelationalTableModel *model;

    //Partie du model du tableau
    QSqlQueryModel *model;
    DatabaseManager *m_db;
    int genreIdx;
    void createMenuBar();

    //partie récuperation de l'identifiant depuis le moniteur serie du tiris
    QSerialPort *serial;

    //partie simulation
    Configuration *sim_config;

    int participantSimId = 0;

};


#endif // GESTIONPARTICIPANT_H
