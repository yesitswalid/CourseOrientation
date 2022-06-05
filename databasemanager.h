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
    genre_id integer,
    rfid integer DEFAULT 0)
    )");

const auto PARTICIPANTS_RACES_SQL = QString(R"(
    create table if not exists participant_races(
    id integer primary key,
    participant_id integer,
    race_id integer,
    finger integer DEFAULT NULL)
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
    location varchar,
    date datetime,
    description varchar)
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
    INSERT INTO races(id_department, name, location, date, description) values(?, ?, ?, ?, ?)
)");


////////////////////////// REQUETE POUR SUPPRIMER UNE LIGNE //////////////////////////////////


const auto DELETE_PARTICIPANT_SQL = QString(R"(
    delete from participants where id = ?
    )");

const auto DELETE_PARTICIPANT_RACE_SQL = QString(R"(
    delete from participant_races where participant_id = ?
    )");

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
    QFileInfo *fileInfo;
    QSqlDatabase m_db;
public:
    ~DatabaseManager();
    //Ouvrir la base de données et effectuer les enregistrement sur course.db
    DatabaseManager();
    //Ouvrir la base de données du fichier dans depuis le nom du constructeur et enregistrer son contenue
    DatabaseManager(QString filePath);

    //PARTIE PARTICIPANT
    void addParticipant(const QString &lastname, const QString &firstname, const QString &mail, const QString &password, const QString &year, int genreId);

    bool isParticipantExist(const QString &mail);
    void insertGenreIfNotExist(const QString &sexe);
    void removeParticipant(const int id);


    void addParticipantRace(int participantId, int raceId);
    bool isParticipantRaceExist(int participantId);


    //PARTIE COURSE
    void addRace(int race_id, int id_department, const QString name, const QString location, const QString date, const QString description);
    bool isRaceExist(const QString &raceName);

    //Copy base de données
    void setDb(QSqlDatabase m_db);
    QSqlDatabase getDb();
    QSqlError initDb();
    QSqlError initDb(QString &connectionName);

};

#endif // DATABASEMANAGER_H
