#include "databasemanager.h"
#include "racemanager.h"
#include <QMessageBox>
#include <QApplication>
#include <QDateTime>

DatabaseManager::DatabaseManager()
{

}

///
/// \brief DatabaseManager::~DatabaseManager
///
DatabaseManager::~DatabaseManager()
{
    m_db.commit();
    m_db.close();
}

///
/// \brief DatabaseManager::setDb
/// \param m_db
///
void DatabaseManager::setDb(QSqlDatabase m_db)
{
    this->m_db = m_db;

}

///
/// \brief DatabaseManager::getDb
/// \return
///
QSqlDatabase DatabaseManager::getDb()
{
    return this->m_db;
}

///
/// \brief DatabaseManager::initDb
/// \return
///
QSqlError DatabaseManager::initDb()
{
    QSqlDatabase db;
    this->setDb(QSqlDatabase::addDatabase("QSQLITE"));
    db = this->getDb();
    db.setDatabaseName(QApplication::instance()->applicationDirPath() + "/course.db");

    if (!db.open())
        return db.lastError();

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
    if(!q.exec(PARTICIPANTS_RACES_DATA_SQL))
       return q.lastError();


    insertGenreIfNotExist(QString("masculin"));
    insertGenreIfNotExist(QString("feminin"));
    insertGenreIfNotExist(QString("Non binaire"));


    return QSqlError();
}

///
/// \brief DatabaseManager::initDb
/// \param connectionName
/// \return
///
QSqlError DatabaseManager::initDb(QString &connectionName)
{
    QSqlDatabase db;
    this->setDb(QSqlDatabase::addDatabase("QSQLITE", connectionName));
    db = this->getDb();
    db.setDatabaseName(QApplication::instance()->applicationDirPath() + "/course.db");

    if (!db.open())
        return db.lastError();

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
    if(!q.exec(PARTICIPANTS_RACES_DATA_SQL))
       return q.lastError();


    insertGenreIfNotExist(QString("masculin"));
    insertGenreIfNotExist(QString("feminin"));
    insertGenreIfNotExist(QString("Non binaire"));


    return QSqlError();
}


/////// PARTIE Participant //////////////////

///
/// \brief DatabaseManager::isParticipantExist
/// \param mail
/// \return
///
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

///
/// \brief DatabaseManager::isParticipantRaceExist
/// \param participantId
/// \return
///
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
///
/// \brief DatabaseManager::addParticipant
/// \param lastname
/// \param firstname
/// \param mail
/// \param password
/// \param year
/// \param genreId
///
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

///
/// \brief DatabaseManager::addParticipantRace
/// \param lastname
/// \param firstname
/// \param mail
/// \param password
/// \param year
/// \param genreId
///
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

///
/// \brief DatabaseManager::addParticipantRace
/// \param participantId
/// \param raceId
///
void DatabaseManager::addParticipantRace(int participantId, int raceId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(INSERT_PARTICIPANT_RACE_SQL);
    q.addBindValue(participantId);
    q.addBindValue(raceId);
    q.exec();

}

///
/// \brief DatabaseManager::getParticipantData
/// \param participantId
/// \return
///
QList<QVariant> DatabaseManager::getParticipantData(int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("SELECT * FROM participants WHERE id=?");
    q.addBindValue(participantId);
    q.exec();

    QList<QVariant> data;
    int i = 0;
    while(q.next())
    {
        qDebug() << q.value(i);
        data.append(q.value(i));
        i++;
    }
    return data;
}


bool DatabaseManager::hasParticipantRaceData(int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("SELECT * FROM participant_races_data WHERE participant_id=? and race_id=?");
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
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

void DatabaseManager::addParticipantRaceData(int participantId, int beacons, int points)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(INSERT_PARTICIPANT_DATA_RACE_SQL);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.addBindValue(beacons);
    q.addBindValue(points);
    q.exec();

}


void DatabaseManager::setPartipantPoints(int participantId, int points)
{
    if(!this->hasParticipantRaceData(participantId))
    {
        this->addParticipantRaceData(participantId, 0, 0);
    }

    QSqlQuery q;
    q = m_db.exec();
    q.prepare("UPDATE participant_races_data SET points=? WHERE race_id=? and participant_id=?");
    q.addBindValue(points);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    qDebug() << q.exec();
}

void DatabaseManager::setPartipantBeacons(int participantId, int beacons)
{
    if(!this->hasParticipantRaceData(participantId))
    {
        this->addParticipantRaceData(participantId, 0, 0);
    }

    QSqlQuery q;
    q = m_db.exec();
    q.prepare("UPDATE participant_races_data SET beacons=? WHERE race_id=? and participant_id=?");
    q.addBindValue(beacons);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}


void DatabaseManager::setDepartTimeParticipant(int participantId, QDateTime startTime)
{

    if(!this->hasParticipantRaceData(participantId))
    {
        this->addParticipantRaceData(participantId, 0, 0);
    }

    QSqlQuery q;
    q = m_db.exec();
    q.prepare("UPDATE participant_races_data SET start=? WHERE race_id=? and participant_id=?");
    q.addBindValue(startTime);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}
void DatabaseManager::setFinishTimeParticipant(int participantId, QDateTime endTime)
{
    if(!this->hasParticipantRaceData(participantId))
    {
        this->addParticipantRaceData(participantId, 0, 0);
    }

    QSqlQuery q;
    q = m_db.exec();
    q.prepare("UPDATE participant_races_data SET end=? WHERE race_id=? and participant_id=?");
    q.addBindValue(endTime);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.addBindValue(participantId);
    q.exec();
}


///
/// \brief DatabaseManager::isFingerExist
/// \param participantId
/// \return
///
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

///
/// \brief DatabaseManager::setFinger
/// \param participantId
/// \param fingerId
///
void DatabaseManager::setFinger(int participantId, QString fingerId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("UPDATE participant_races SET finger=? WHERE participant_id=? and race_id=?");
    q.addBindValue(fingerId);
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.exec();
}


///
/// \brief DatabaseManager::isPortiqueBIDExist
/// \param participantId
/// \return
///
bool DatabaseManager::isPortiqueBIDExist(int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("SELECT * FROM participant_races WHERE participant_id=? and race_id=? and bid IS NOT NULL");
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
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

///
/// \brief DatabaseManager::setPortiqueBID
/// \param participantId
/// \param bid
///
void DatabaseManager::setPortiqueBID(int participantId, QString bid)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare("UPDATE participant_races SET bid=? WHERE participant_id=? and race_id=?");
    q.addBindValue(bid);
    q.addBindValue(participantId);
    q.addBindValue(RaceManager::getInstance()->getRaceId());
    q.exec();
}

///
/// \brief DatabaseManager::insertGenreIfNotExist
/// \param sexe
///
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

///
/// \brief DatabaseManager::removeParticipant
/// \param id
///
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

///
/// \brief DatabaseManager::isRaceExist
/// \param raceName
/// \return
///
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

///
/// \brief DatabaseManager::addRace
/// \param race_id
/// \param id_department
/// \param name
/// \param date
/// \param location
/// \param gps_longitude
/// \param gps_latitude
/// \param difficulty
/// \param type
/// \param book
///
void DatabaseManager::addRace(int race_id, int id_department, QString name, QDateTime date, QString location,
                              QString gps_longitude, QString gps_latitude, int difficulty, int type, int book)
{
      QSqlQuery q;
      q = m_db.exec();
      q.prepare(INSERT_RACE_SQL);

      q.addBindValue(race_id);
      q.addBindValue(id_department);
      q.addBindValue(name);
      q.addBindValue(date);
      q.addBindValue(location);
      q.addBindValue(gps_longitude);
      q.addBindValue(gps_latitude);
      q.addBindValue(difficulty);
      q.addBindValue(type);
      q.addBindValue(book);

      qDebug() << q.exec();
}

///
/// \brief DatabaseManager::addParticipantCheckpoint
/// \param altitude
/// \param longitude
/// \param attitude
/// \param raceId
/// \param participantId
/// \param orderId
/// \param points
///
void DatabaseManager::addParticipantCheckpoint(QString altitude, QString longitude, QString attitude, int raceId, int participantId, int orderId, int points)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(INSERT_CHECKPOINT_SQL);

    q.addBindValue(altitude);
    q.addBindValue(longitude);
    q.addBindValue(attitude);
    q.addBindValue(raceId);
    q.addBindValue(participantId);
    q.addBindValue(orderId);
    q.addBindValue(points);
    qDebug() << q.exec();

}


///
/// \brief DatabaseManager::hasParticipantCheckpoint
/// \param orderId
/// \param raceId
/// \param participantId
/// \return
///
bool DatabaseManager::hasParticipantCheckpoint(int orderId, int raceId, int participantId)
{
    QSqlQuery q;
    q = m_db.exec();
    q.prepare(SELECT_ORDER_CHECKPOINT_PARTICIPANT);
    q.addBindValue(orderId);
    q.addBindValue(raceId);
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

