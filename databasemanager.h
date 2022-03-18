#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H


#include <QtSql>
#include <QDate>
#include <QString>
#include <QCryptographicHash>

///////////////////////////// CREER UNE TABLE //////////////////////////////////////////////////

const auto PARTICIPANTS_SQL = QString(R"(
    create table if not exists participants(id integer primary key, lastname varchar, firstname varchar, mail varchar, password varchar, date_birthday varchar,
                       genre_id integer)
    )");

const auto GENRES_SQL = QString(R"(
    create table if not exists genres(id integer primary key, sexe varchar)
    )");


//////////////////// REQUETE INSERTION /////////////////
const auto INSERT_PARTICIPANT_SQL = QString(R"(
    insert into participants(lastname, firstname, mail, password, date_birthday, genre_id)
                      values(?, ?, ?, ?, ?, ?)
    )");

const auto INSERT_GENRE_SQL = QString(R"(
    INSERT OR REPLACE INTO genres (sexe) values(?);
)");


////////////////////////// REQUETE POUR SUPPRIMER UNE LIGNE //////////////////////////////////


const auto DELETE_PARTICIPANT_SQL = QString(R"(
    delete from participants where id = ?
    )");

const auto DELETE_GENRE_SQL = QString(R"(
    delete from genres where id = ?
    )");

///////////////////////////////// REQUETE SUPRIMER UNE TABLE //////////////////////////////////

const auto DROP_PARTICIPANT_SQL = QString(R"(
    DROP TABLE IF EXISTS participants
)");

const auto DROP_GENRE_SQL = QString(R"(
    DROP TABLE IF EXISTS genres
    )");


///////////////////////////// REQUETE SELECT ///////////////////////////////////////////////////
const auto SELECT_GENRE_SEXE = QString(R"(SELECT * FROM genres WHERE sexe=?)");


const auto SELECT_DATA_PARTICIPANT = QString(R"(SELECT * FROM participants WHERE mail=?)");

/////////////////////////////////////////////////////////////////////////////////////////////////////////

class DatabaseManager
{
public:
    DatabaseManager();
    void addParticipant(const QString &lastname, const QString &firstname, const QString &mail, const QString &password, const QString &date_birthday, int genreId);
    void insertGenreIfNotExist(const QString &sexe);
    void removeParticipant(const int id);
    QSqlError initDb();

};

#endif // DATABASEMANAGER_H
