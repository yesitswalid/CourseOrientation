#ifndef GESTIONCOURSE_H
#define GESTIONCOURSE_H

#include <QSqlError>
#include <QMainWindow>
#include <QtSql>
#include <QtWidgets>
#include <ui_gestioncourse.h>
#include "databasemanager.h"

class GestionCourse : public QMainWindow
{
    Q_OBJECT

public:
     GestionCourse();
    ~GestionCourse();
    void createTableView();
    void init();

private slots:

    void on_continueButton_clicked();

private:
    Ui::GestionCourse ui;
    void showError(const QSqlError &err);
    QSqlRelationalTableModel *model;
    DatabaseManager *m_db;
    int genreIdx;
    void createMenuBar();
};


#endif // GESTIONPARTICIPANT_H
