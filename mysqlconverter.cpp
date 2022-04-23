#ifndef MYSQLCONVERTER_CPP
#define MYSQLCONVERTER_CPP

#include <QMap>
#include <QString>
#include <QDebug>
#include <QtSql>
#include <QProcess>
#include <QJsonObject>

class MySQLConverter {


public:
    QSqlDatabase *m_db;   
    QJsonObject *m_data;

MySQLConverter()
{    

    QSqlDatabase m_dbCopy = QSqlDatabase::addDatabase("QMYSQL", "myMysql");

    m_dbCopy.setHostName("127.0.0.1");
    m_dbCopy.setDatabaseName("coursorient");
    m_dbCopy.setUserName("root");
    m_dbCopy.setPassword("walid13");

    if (!m_dbCopy.open())
    {
        qDebug() << "Erreur lors de la connexion du serveur : " << m_dbCopy.lastError();
    }

    m_db = new QSqlDatabase(m_dbCopy); // initialise le pointer avec un nouveau objet d'une copie de QSqlDatabase
    qDebug() << m_db->open();
}

MySQLConverter(QString username, QString password, QString hostname, QString dbName)
{
    QSqlDatabase m_dbCopy = QSqlDatabase::addDatabase("QMYSQL", "myMysql");

    m_dbCopy.setUserName(username);
    m_dbCopy.setPassword(password);
    m_dbCopy.setHostName(hostname);
    m_dbCopy.setDatabaseName(dbName);

    if (!m_dbCopy.open())
    {
        qDebug() << "Erreur lors de la connexion du serveur : " << m_dbCopy.lastError();
    }

    m_db = new QSqlDatabase(m_dbCopy); // initialise le pointer avec un nouveau objet d'une copie de QSqlDatabase
    m_dbCopy.close();

    if(m_db->open())
    {
        qDebug() << "Le serveur communique avec la base de données.";
    } else {
        qWarning() << "Le serveur ne communique pas avec la base de données !";
    }

    QStringList tables = m_db->tables();
    qDebug() << tables.count();
      for (int i=0; i< tables.count(); ++i) {
          qDebug() << tables[i];
      }

    //Constructeur
}

~MySQLConverter()
{
    //Destructeur
    delete m_db;
}


QSqlDatabase *getDatabase()
{
    return this->m_db;
}

void exportSql(QString name)
{
    QProcess dumpProcess;
    QStringList args;
    args << "-uroot" << "-pmysql" << "test";
    dumpProcess.setStandardOutputFile(name + " .sql");
    dumpProcess.start("mysqldump", args); //mysqldump requis !!!!!! dans l'environnement
}

QMap<QString, QString> SqlDataToMap()
{
    QSqlQuery q = m_db->exec();
    q.prepare("SHOW TABLES");
    q.exec();

    QVector<QString> tables;
    while(q.next())
    {
        tables.append(q.value(0).toString());
    }

    for (QString table : tables)
    {
        qDebug() << table;
    }

    QMap<QString, QString>parsed;
    parsed["test"] = "d";
    return parsed;
    }
};

#endif
