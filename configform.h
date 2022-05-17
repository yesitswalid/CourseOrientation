#ifndef CONFIGFORM_H
#define CONFIGFORM_H

#include <ui_configform.h>
#include <QWidget>
#include <configuration.cpp>
#include <QSqlDatabase>
#include <QJsonObject>

class ConfigForm : public QWidget
{
    Q_OBJECT

public:
    explicit ConfigForm();
    ~ConfigForm();
    QJsonObject getDatabaseData();

private slots:
    void on_continueButton_clicked();
    void on_testButton_clicked();

private:
    Ui::ConfigForm ui;
    QSqlDatabase *m_db;
    Configuration *configuration;
};

#endif // CONFIGFORM_H
