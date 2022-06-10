#include "databasemanager.h"
#include "racemanager.h"
#include <QMessageBox>


DatabaseManager::DatabaseManager()
{
    this->fileInfo = new QFileInfo();
    this->fileInfo->setFile("course.db");
}

DatabaseManager::DatabaseManager(QString filePath)
{
    this->fileInfo = new QFileInfo();
    this->fileInfo->setFile(filePath);
}

DatabaseManager::~DatabaseManager()
{
    delete this->fileInfo;
}

void DatabaseManager::setDb(QSqlDatabase m_db)
{
    this->m_db = m_db;

}

QSqlDatabase DatabaseManager::getDb()
{
    return this->m_db;
}

QSqlError DatabaseManager::initDb()
{
    QSqlDatabase db;
    this->setDb(QSqlDatabase::addDatabase("QSQLITE"));
    db = this->getDb();
    db.setDatabaseName(this->fileInfo->absoluteFilePath());

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("mail", Qt::CaseInsensitive)
        && tables.contains("nom", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(PARTICIPANTS_SQL))
        return q.lastError();
    if (!q.exec(GENDERS_SQL))
        return q.lastError();
    if (!q.exec(RACES_SQL))
        return q.lastError();
    if(!q.exec(PARTICIPANTS_RACES_SQL))
        return q.lastError();
    if (!q.exec(CHECKPOINTS_SQL))
        return q.lastError();
    if (!q.exec(ARRIVALS_SQL))
        return q.lastError();
    if(!q.exec(PARTICIPANTS_RACES_DATA_SQL))
       return q.lastError();


    insertGenreIfNotExist(QString("masculin"));
    insertGenreIfNotExist(QString("feminin"));
    insertGenreIfNotExist(QString("Non binaire"));


    return QSqlError();
}


QSqlError DatabaseManager::initDb(QString &connectionName)
{
    QSqlDatabase db;
    this->setDb(QSqlDatabase::addDatabase("QSQLITE", connectionName));
    db = this->getDb();
    db.setDatabaseName(this->fileInfo->absoluteFilePath());

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("mail", Qt::CaseInsensitive)
        && tables.contains("nom", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(PARTICIPANTS_SQL))
        return q.lastError();
    if (!q.exec(GENDERS_SQL))
        return q.lastError();
    if (!q.exec(RACES_SQL))
        return q.lastError();
    if(!q.exec(PARTICIPANTS_RACES_SQL))
        return q.lastError();
    if (!q.exec(CHECKPOINTS_SQL))
        return q.lastError();
    if (!q.exec(ARRIVALS_SQL))
        return q.lastError();
    if(!q.exec(PARTICIPANTS_RACES_DATA_SQL))
       return q.lastError();


    insertGenreIfNotExist(QString("masculin"));
    insertGenreIfNotExist(QString("feminin"));
    insertGenreIfNotExist(QString("Non binaire"));


    return QSqlError();
}

bool DatabaseManager::isParticipantExist(const QString &mail)
{
     QSqlQuery q;
     q = m_db.exec();
     q.prepare(SELECT_DATA_PARTICIPANT);
     q.addBindValue(mail);
     if(q.exec())
     {
        return q.next();
     }
     int recCount = 0;
     while(q.next())
     {
      recCount++;
     }
     return  (recCount > 0);
}


bool DatabaseManager::isParticipantRaceExist(int participantId)
{
     QSqlQuery q;
     q = m_db.exec();
     q.prepare(SELECT_DATA_EXIST_PARTICIPANT_RACE);
     q.addBindValue(participantId);
     if(q.exec())
     {
        return q.next();
     }
     int recCount = 0;
     while(q.next())
     {
      recCount++;
     }
     return  (recCount > 0);
}



//Ajouter un participant dans la table participants
void DatabaseManager::addParticipant(const QString &lastname, const QString &firstname,
                                     const QString &mail,
                                     const QString &password,
                                     const QString &year,
                                     int genreId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(INSERT_PARTICIPANT_SQL);
    q.addBindValue(lastname);
    q.addBindValue(firstname);
    q.addBindValue(mail);
    q.addBindValue(password);
    q.addBindValue(year);
    q.addBindValue(genreId);
    q.exec();

}

//Ajouter un participant dans la table participant et récupérer l'id du participant pour l'insérer dans participant_race
void DatabaseManager::addParticipantRace(const QString &lastname, const QString &firstname,
                                          const QString &mail,
                                          const QString &password,
                                          const QString &year,
                                          int genreId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(INSERT_PARTICIPANT_SQL);
    q.addBindValue(lastname);
    q.addBindValue(firstname);
    q.addBindValue(mail);
    q.addBindValue(password);
    q.addBindValue(year);
    q.addBindValue(genreId);
    q.exec();

    this->addParticipantRace(q.lastInsertId().toInt(), RaceManager::getInstance()->getRaceId());

}

/*
int DatabaseManager::getParticipantData(QString email)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(SELECT_DATA_PARTICIPANT);
    q.addBindValue(email);
    q.exec();

    return q

}
*/
void DatabaseManager::addParticipantRace(int participantId, int raceId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(INSERT_PARTICIPANT_RACE_SQL);
    q.addBindValue(participantId);
    q.addBindValue(raceId);
    q.exec();

}

QList<QVariant> DatabaseManager::getParticipantData(int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("SELECT * FROM participants as p, participant_races as pr, participant_races_data as prc WHERE p.id=pr.participant_id=prc.participant_id=?");
    q.addBindValue(participantId);
    q.exec();

    QList<QVariant> data;
    int i = 0;
    while(q.next())
    {
        data.append(q.value(i));
        i++;
    }
    return data;
}

bool DatabaseManager::isFingerExist(int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("SELECT * FROM participant_races WHERE participant_id=? and finger IS NOT NULL");
    q.addBindValue(participantId);
    if(q.exec())
    {
       return q.next();
    }
    int recCount = 0;
    while(q.next())
    {
     recCount++;
    }
    return  (recCount > 0);
}



bool DatabaseManager::isPortiqueRFIDExist(int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("SELECT * FROM participant_races WHERE participant_id=? and rfid IS NOT NULL");
    q.addBindValue(participantId);
    if(q.exec())
    {
       return q.next();
    }
    int recCount = 0;
    while(q.next())
    {
     recCount++;
    }
    return  (recCount > 0);
}


void DatabaseManager::insertGenreIfNotExist(const QString &sexe)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(SELECT_GENRE_SEXE);
    q.addBindValue(sexe);
    if(q.exec()){
        if(!q.next())
        {
           q.prepare(INSERT_GENDER_SQL);
           q.addBindValue(sexe);
           q.exec();
        }
    }
}

void DatabaseManager::removeParticipant(const int id)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(DELETE_PARTICIPANT_SQL);
    q.addBindValue(id);
    q.exec();

    q.prepare(DELETE_PARTICIPANT_RACE_SQL);
    q.addBindValue(id);
    q.exec();

    q.prepare(DELETE_PARTICIPANT_RACE_DATA_SQL);
    q.addBindValue(id);
    q.exec();

}


////////////////////////////////////////////////////////////////////////////PART RACE

bool DatabaseManager::isRaceExist(const QString &raceName)
{
     QSqlQuery q;
     q = m_db.exec();
     q.prepare(SELECT_DATA_RACE);
     q.addBindValue(raceName);
     if(q.exec())
     {
        return q.next();
     }
     int recCount = 0;
     while(q.next())
     {
      recCount++;
     }
     return  (recCount > 0);
}

void DatabaseManager::addRace(int race_id, int id_department, const QString name, const QString location, const QString date, const QString description)
{
      QSqlQuery q;
      q = m_db.exec();
      q.prepare(INSERT_RACE_SQL);
      q.addBindValue(race_id);
      q.addBindValue(id_department);
      q.addBindValue(name);
      q.addBindValue(location);
      q.addBindValue(date);
      q.addBindValue(description);
      q.exec();
}
