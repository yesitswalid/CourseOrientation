#ifndef MYSQLCONVERTER_CPP
#define MYSQLCONVERTER_CPP

#include <QMap>
#include <QString>
#include <QDebug>
#include <QtSql>
#include <QProcess>
#include <QJsonObject>
#include <QMessageBox>
#include <databasemanager.h>

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


int getConvertFormatSexe(QString sexe)
{
    if(sexe == "M") {
        return 1;
    }
    else if(sexe == "F")
    {
        return 2;
    }
    return 3;
}
/*
const auto PARTICIPANTS_SQL = QString(R"(
    create table if not exists participants(id integer primary key, lastname varchar, firstname varchar, mail varchar, password varchar, year varchar,
                       genre_id integer, rfid integer DEFAULT 0)
    )");

const auto GENDERS_SQL = QString(R"(
    create table if not exists genders(id integer primary key, sexe varchar)
    )");


const auto RACES_SQL = QString(R"(
    create table if not exists races(id integer primary key, name varchar, datetime vachar, description varchar)
    )");

const auto CHECKPOINTS_SQL = QString(R"(
    create table if not exists checkpoints(id integer primary key, altitude varchar, longitude vachar, attitude varchar, race_id integer, order_id varchar, points integer)
)");


const auto ARRIVALS_SQL = QString(R"(
    create table if not exists checkpoints(id integer primary key, participant_id integer, checkpoint_id integer, datetime varchar)
)");

*/

bool importData(QString dirPath)
{
    QDir dir;
    QFileInfo fileInfo;
    QString connectionName;
    QSqlQuery query;

    QVector<int>race_ids;


    query = m_db->exec();

    connectionName = "myNewDb";
    dir.setPath(dirPath);
    query.prepare(QString("SELECT * FROM races"));

    if (!query.exec()) {
        return false;
    }

    while (query.next())
    {
       race_ids.push_back(query.value(0).toInt());
    }

    for(int race_id : race_ids)
    {
       query.prepare(QString("SELECT email, password, firstname, lastname, year, gender FROM participants"));

       if (!query.exec())
       {
           return false;
       }

       fileInfo.setFile(dir.path() + "/course_" + QString::number(race_id) + ".db");
       qDebug() << fileInfo.path();
       if(!fileInfo.exists())
       {
             DatabaseManager m_db_saver = DatabaseManager(dir.path() + "/course_" + QString::number(race_id) + ".db");
             QSqlError err = m_db_saver.initDb(connectionName);

             if (err.type() != QSqlError::NoError) {
                 return false;
             }

             while(query.next())
             {
                     QString email = query.value(0).toString();
                     QString password = query.value(1).toString();
                     QString firstname = query.value(2).toString();
                     QString lastname = query.value(3).toString();
                     QString year = query.value(4).toString();
                     int genderId = getConvertFormatSexe(query.value(5).toString());

                     if(!m_db_saver.isParticipantExist(email))
                     {
                         m_db_saver.addParticipant(lastname, firstname, email, password, year, genderId);
                     }
            }

             query.prepare(QString("SELECT id_department, name, date, location FROM races"));

             if (!query.exec())
             {
                 return false;
             }

             while(query.next())
             {
                 int id_department = query.value(0).toInt();
                 QString raceName = query.value(1).toString();
                 QString date = query.value(2).toString();
                 QString location = query.value(3).toString();

                 qDebug() << id_department << raceName << date << location;

                 if(!m_db_saver.isRaceExist(raceName))
                 {
                     m_db_saver.addRace(id_department, raceName, location, date, "N/A");
                 }
             }

             //ToDo : finish
            m_db_saver.getDb().removeDatabase(connectionName);
            m_db_saver.getDb().close();
        }
    }
    return true;
}


void exportSql(QString filePath)
{
    QProcess dumpProcess;
    QStringList args;
    //args << "-uroot" << "-pmysql" << "test";
    args << "-uroot" << "-pmysql" << filePath;
    dumpProcess.setStandardOutputFile(filePath);
    dumpProcess.start("mysqldump", args); //mysqldump requis !!!!!! dans l'environnement
    dumpProcess.waitForFinished(); //Attendre que le processus soit entirement finit.
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

//Date time save
/*
 * query.prepare("INSERT INTO table (table_date_time, ...) "
        " VALUES (:datetime, ...)");
QDateTime timestamp = QDateTime::currentDateTime();
timestamp.addDays(1);
// Qt::ISODate format...
query.bindValue(":datetime", timestamp);// 2017-09-05T11:50:39
// Custom format
query.bindValue(":datetime", timestamp.toString("yyyy-MM-dd hh:mm:ss"));  // same as above but without the T
*/
};

#endif
