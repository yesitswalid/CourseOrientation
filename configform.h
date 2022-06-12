#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <ui_configform.h>
#include <QWidget>
#include <configuration.cpp>
#include <QSqlDatabase>
#include <QJsonObject>
#include <mysqldata.cpp>

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
     explicit ConfigForm();
    ~ConfigForm();
     QJsonObject getDatabaseData();
     void init(MySQLData *data);

private slots:
    void on_continueButton_clicked();
    void on_testButton_clicked();

private:
    Ui::ConfigForm ui;
    QWidget *app;
    Configuration *configuration;
    MySQLData *m_db;
};

#endif // CONFIGFORM_H
