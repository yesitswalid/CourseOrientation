#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QtSql>
#include <QDate>
#include <QString>
#include <QCryptographicHash>

///////////////////////////// CREER UNE TABLE //////////////////////////////////////////////////

const auto PARTICIPANTS_SQL = QString(R"(
    create table if not exists participants(
    id integer primary key,
    lastname varchar,
    firstname varchar,
    mail varchar,
    password varchar,
    year varchar,
    genre_id integer)
    )");

const auto PARTICIPANTS_RACES_SQL = QString(R"(
    create table if not exists participant_races(
    id integer primary key,
    participant_id integer,
    race_id integer,
    finger integer DEFAULT NULL,
    rfid integer DEFAULT NULL)
    )");

const auto PARTICIPANTS_RACES_DATA_SQL = QString(R"(
    create table if not exists participant_races_data(
    id integer primary key,
    participant_id integer,
    race_id integer,
    beacons integer DEFAULT NULL,
    points integer DEFAULT 0,
    race_duration integer DEFAULT 0)
    )");

const auto GENDERS_SQL = QString(R"(
    create table if not exists genders(
    id integer primary key,
    sexe varchar)
    )");

const auto RACES_SQL = QString(R"(
  create table if not exists races(
      id integer primary key,
      id_department integer,
      name varchar,
      date datetime DEFAULT NULL,
      location varchar DEFAULT NULL,
      gps_longitude varchar,
      gps_latitude varchar,
      difficulty integer,
      type integer,
      book integer)
)");


const auto CHECKPOINTS_SQL = QString(R"(
    create table if not exists checkpoints(
    id integer primary key,
    altitude varchar,
    longitude vachar,
    attitude varchar,
    race_id integer,
    order_id varchar,
    points integer)
)");


const auto ARRIVALS_SQL = QString(R"(
    create table if not exists arrivals(
    id integer primary key,
    participant_id integer,
    checkpoint_id integer,
    datetime varchar)
)");




//////////////////// REQUETE INSERTION /////////////////
const auto INSERT_PARTICIPANT_SQL = QString(R"(
    insert into participants(lastname, firstname, mail, password, year, genre_id)
                      values(?, ?, ?, ?, ?, ?)
    )");

const auto INSERT_PARTICIPANT_RACE_SQL = QString(R"(
    insert into participant_races(participant_id, race_id)
                      values(?, ?)
    )");

const auto INSERT_GENDER_SQL = QString(R"(
    INSERT OR REPLACE INTO genders (sexe) values(?);
)");

const auto INSERT_RACE_SQL = QString(R"(
   INSERT INTO races (id, id_department, name, date, location, gps_longitude, gps_latitude, difficulty, type, book) values(?,?,?,?,?,?,?,?,?,?)
)");

////////////////////////// REQUETE POUR SUPPRIMER UNE LIGNE //////////////////////////////////


const auto DELETE_PARTICIPANT_SQL = QString(R"(
    delete from participants where id = ?
    )");

const auto DELETE_PARTICIPANT_RACE_SQL = QString(R"(
    delete from participant_races where participant_id = ?
    )");


const auto DELETE_PARTICIPANT_RACE_DATA_SQL = QString(R"(
    delete from participant_races_data where participant_id = ?
")");

const auto DELETE_GENDER_SQL = QString(R"(
    delete from genders where id = ?
    )");

///////////////////////////////// REQUETE SUPRIMER UNE TABLE //////////////////////////////////

const auto DROP_PARTICIPANT_SQL = QString(R"(
    DROP TABLE IF EXISTS participants
)");

const auto DROP_GENDER_SQL = QString(R"(
    DROP TABLE IF EXISTS genders
    )");


///////////////////////////////// REQUETE SELECT ///////////////////////////////////////////////////
const auto SELECT_GENRE_SEXE = QString(R"(SELECT * FROM genders WHERE sexe=?)");

const auto SELECT_DATA_PARTICIPANT = QString(R"(SELECT * FROM participants WHERE mail=?)");

const auto SELECT_DATA_EXIST_PARTICIPANT_RACE = QString(R"(SELECT * FROM participant_races WHERE participant_id=?)");

const auto SELECT_DATA_PARTICIPANT_RACE = QString(R"(SELECT * FROM participant_races WHERE race_id=?)");

const auto SELECT_DATA_RACE = QString(R"(SELECT * FROM races WHERE name=?)");

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DatabaseManager
{
private:
    QSqlDatabase m_db;
public:
    ~DatabaseManager();
    //Ouvrir la base de données et effectuer les enregistrement sur course.db
    DatabaseManager();
    //Ouvrir la base de données du fichier dans depuis le nom du constructeur et enregistrer son contenue
    //DatabaseManager(QString filePath);

    //PARTIE PARTICIPANT
    void addParticipant(const QString &lastname, const QString &firstname, const QString &mail, const QString &password, const QString &year, int genreId);

    bool isParticipantExist(const QString &mail);
    void insertGenreIfNotExist(const QString &sexe);
    void removeParticipant(const int id);


    void addParticipantRace(const QString &lastname, const QString &firstname,
                                              const QString &mail,
                                              const QString &password,
                                              const QString &year,
                                              int genreId);
    void addParticipantRace(int participantId, int raceId);


    bool isParticipantRaceExist(int participantId);

    QList<QVariant> getParticipantData(int participantId);


    //PARTIE DOSSAR PASSAGE ET DOIGT(RFID)

    bool isPortiqueBIDExist(int participantId);
    bool isFingerExist(int participantId);

    void setPortiqueBID(int participantId, qlonglong bid);
    void setFinger(int participantId, qlonglong fingerId);


    //PARTIE COURSE
    //void addRace(int race_id, int id_department, const QString name, const QString location, QDateTime date);
    void addRace(int race_id, int id_department, QString name, QDateTime date, QString location,
                  QString gps_longitude, QString gps_latitude, int difficulty, int type, int book);

    bool isRaceExist(const QString &raceName);

    //Copy base de données
    void setDb(QSqlDatabase m_db);
    QSqlDatabase getDb();
    QSqlError initDb();
    QSqlError initDb(QString &connectionName);

};

#endif // DATABASEMANAGER_H
