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

private:
    Ui::GestionParticipant ui;
    void showError(const QSqlError &err);
    QSerialPort *serial;
    //QSqlRelationalTableModel *model;
    QSqlQueryModel *model;
    DatabaseManager *m_db;
    int genreIdx;
    void createMenuBar();

};


#endif // GESTIONPARTICIPANT_H
