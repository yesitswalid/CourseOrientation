#ifndef SQLITECONVERTER_CPP
#define SQLITECONVERTER_CPP

#include <QMap>
#include <QString>
#include <QDebug>
#include <QtSql>

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
    QSqlDatabase dbCopy = QSqlDatabase::addDatabase("QSQLITE", "mySqlite1");
    dbCopy.setDatabaseName(p);
    m_db = new QSqlDatabase(dbCopy);

    this->path = QDir::currentPath() + "/" + p;

    if (!m_db->open())
    {
        qDebug() << "Erreur lors de l'ouverture du fichier : " << m_db->lastError();
    }

    //Constructeur
}

~SQLiteConverter()
{
    //Destructeur
    delete m_db;
}



bool exportDbTableSchemaToFile(
       const QString &t_createTableStatementSeperator /* = QString("--;;")  */
    )
    {
       QString t_fileName;
       t_fileName = m_db->databaseName();
       bool r_dumpingSuccessfull = false;

       if( m_db->open())
       {
          qDebug() << "Dump de la base de données et du schema de "+ t_fileName;

          QFile outputFile(t_fileName);
          outputFile.open(QIODevice::WriteOnly);

          if( !outputFile.isOpen() )
          {
             qDebug() << "- Erreur lors de l'ouverture du fichier '" << t_fileName << "' pour pouvoir dump SQL pour les tables de creation!";
             return r_dumpingSuccessfull;
          }


          QTextStream outStream(&outputFile);

          QSqlQuery sqlite_masterQuery;
          sqlite_masterQuery = m_db->exec();

          if(!sqlite_masterQuery.exec("SELECT * FROM sqlite_master"))
          {
             QString lastError = sqlite_masterQuery.lastError().text();
             qDebug() << lastError;
             return r_dumpingSuccessfull;
          }
          else
          {
             do
             {
                QString tableName = sqlite_masterQuery.value("name").toString();
                if( sqlite_masterQuery.value("type").toString() == "table" && tableName != "sqlite_sequence" )
                {
                   outStream << sqlite_masterQuery.value("sql").toString();
                   outStream << "\n";
                   outStream << t_createTableStatementSeperator;
                   outStream << "\n";

                   r_dumpingSuccessfull = true;
                }
             } while( sqlite_masterQuery.next() );
          }

          outputFile.close();
       }

       return r_dumpingSuccessfull;
}

bool exportSql()
{

    /*
    QProcess dumpProcess;
    QStringList args;
    QString cmd;

    cmd = "sqlite3 " + QString('"') + path + QString('"') + " .dump > " + filePath;
    args << path + " .dump > " + filePath;
    dumpProcess.setStandardOutputFile(filePath); //avertir la sortie d'un fichier pour permettre la creation du fichier
    dumpProcess.startCommand(cmd); //sqlite3 requis !!!!!! dans l'environnement (Executer la commande suivante dans un nouveau processus sur un invite de command ou terminal)
    dumpProcess.waitForFinished(); //Attendre que le processus soit entirement finit.
    */

    //Export all data information to serveur web

    return true;

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
