#ifndef APPLICATION_H
#define APPLICATION_H

#include <QMainWindow>
#include <QSqlError>
#include <configuration.cpp>
#include <gestionparticipant.h>
#include <inscriptionform.h>
#include <statistique.h>
#include <databasemanager.h>
#include <configform.h>

QT_BEGIN_NAMESPACE
namespace Ui { class Application; }
QT_END_NAMESPACE

class Application : public QMainWindow
{
    Q_OBJECT

public:
    Application(QWidget *parent = nullptr);
    ~Application();
    GestionParticipant *gestion_participant;
    InscriptionForm *inscription_form;
    Statistique *statistique;
    ConfigForm *config_form;
    DatabaseManager *m_db;
    Configuration *configuration;

private slots:
    void on_actionConfiguration_triggered();

    void on_actionInscription_triggered();

    void on_actionGestion_des_participants_triggered();

    void on_actionStatistique_triggered();

    void on_actionBddConfig_triggered();

private:
    Ui::Application *ui;
    void init();
    void showError(const QSqlError &err);
};
#endif // APPLICATION_H
