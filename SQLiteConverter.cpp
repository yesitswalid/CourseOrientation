#include <QMap>
#include <QString>
#include <QDebug>
#include <QtSql>

class SQLiteConverter {


public:
    QSqlDatabase *m_db;



SQLiteConverter()
{
    m_db = new QSqlDatabase();
    m_db->addDatabase("QSQLITE");
}

SQLiteConverter(QString path)
{
    m_db = new QSqlDatabase();
    m_db->addDatabase("QSQLITE");
    m_db->setDatabaseName(path);

    if (!m_db->open())
    {
        qDebug() << "Erreur lors de l'ouverture du fichier : " << m_db->lastError();
    }

    //Constructeur
}

~SQLiteConverter()
{
    //Destructeur
}


QSqlDatabase *getDatabase()
{
    return this->m_db;
}

QMap<QString, QString> SqlDataToMap()
{

    for (int i = 0; i < (int)sizeof(m_db->tables()); i++)
    {
        qDebug() << m_db->tables().at(i).toUtf8();
    }
    /*
    for (QString table : m_db->tables())
    {
        qDebug() << table;
    }
    */

    QMap<QString, QString>parsed;
    parsed["test"] = "d";
    return parsed; 
}


};
