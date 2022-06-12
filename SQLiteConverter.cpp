#ifndef SQLITECONVERTER_CPP
#define SQLITECONVERTER_CPP

#include <QMap>
#include <QString>
#include <QDebug>
#include <QtSql>
#include <QApplication>

class SQLiteConverter {


public:
    QSqlDatabase *m_db;
    QString path;



SQLiteConverter()
{
    m_db = new QSqlDatabase();
    m_db->addDatabase("QSQLITE", "mySqlite");
}

SQLiteConverter(QString p)
{
    //Constructeur

    this->path = QApplication::applicationDirPath() + "/" + p;

    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "mySqlite1"));
    m_db->setDatabaseName(this->path);

    if (!m_db->open())
    {
        qDebug() << "Erreur lors de l'ouverture du fichier : " << m_db->lastError();
    }
}

bool exportSql()
{
    //Donnes a transmettre au serveur web
    return true;
}

~SQLiteConverter()
{
    //Destructeur
    delete m_db;
}


QSqlDatabase *getDatabase()
{
    return this->m_db;
}


void setPath(QString path)
{
    this->path = path;
}

QString getPath()
{
    return this->path;
}


};

#endif
