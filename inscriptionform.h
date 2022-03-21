#ifndef INSCRIPTIONFORM_H
#define INSCRIPTIONFORM_H

#include <QtWidgets>
#include <ui_inscriptionform.h>
#include <databasemanager.h>

class InscriptionForm : public QMainWindow
{
    Q_OBJECT

public:
    InscriptionForm();
    ~InscriptionForm();
    void Register();
    int getSexe();
    void clearAllInputs();

private slots:
    void on_inscriptionButton_clicked();

private:
    Ui::InscriptionForm ui;
    bool isFormEmpty();
    QString errorMessage();
    DatabaseManager *m_db;
};

#endif // INSCRIPTIONFORM_H
