#ifndef GESTIONPARTICIPANT_H
#define GESTIONPARTICIPANT_H

#include <QSqlError>
#include <QMainWindow>
#include <QtSql>
#include <QtWidgets>
#include <ui_gestionparticipant.h>
#include "databasemanager.h"

class GestionParticipant : public QMainWindow
{
    Q_OBJECT

public:
     GestionParticipant();
    ~GestionParticipant();
    void init();

private slots:
    void on_suprButton_clicked();

private:
    Ui::GestionParticipant ui;
    void showError(const QSqlError &err);
    QSqlRelationalTableModel *model;
    DatabaseManager *m_db;
    int genreIdx;
    void createMenuBar();
};


#endif // GESTIONPARTICIPANT_H
