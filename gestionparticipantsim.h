#ifndef GESTIONPARTICIPANTSIM_H
#define GESTIONPARTICIPANTSIM_H

#include <QSqlError>
#include <QMainWindow>
#include <QtSql>
#include <QtWidgets>
#include <QByteArray>
#include <ui_gestionparticipantsim.h>
#include "databasemanager.h"
#include <QSerialPort>

class GestionParticipantSim : public QMainWindow
{
    Q_OBJECT

public:
     GestionParticipantSim();
     GestionParticipantSim(DatabaseManager *db);
    ~GestionParticipantSim();
    void createTableView();
    void init();

private slots:
    void on_suprButton_clicked();

    void on_updateButton_clicked();

    void on_razButton_clicked();

    void razGetData(QByteArray);

    void finishedRead();

    void serialReceived();

private:
    Ui::GestionParticipantSim ui;
    void showError(const QSqlError &err);
    QSerialPort *serial;
    //QSqlRelationalTableModel *model;
    QSqlQueryModel *model;
    DatabaseManager *m_db;
    int genreIdx;
    void createMenuBar();

};


#endif // GESTIONPARTICIPANTSIM_H
