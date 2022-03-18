#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

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

void addParticipant(QSqlQuery &q, const QString &lastname, const QString &firstname, const QString &mail, const QString &password, const QString &date_birthday, int genreId)
{
    q.prepare(INSERT_PARTICIPANT_SQL);

    q.addBindValue(lastname);
    q.addBindValue(firstname);
    q.addBindValue(mail);
    q.addBindValue(password);
    q.addBindValue(date_birthday);
    q.addBindValue(genreId);
    q.exec();
}

void insertGenreIfNotExist(const QString &sexe)
{
    QSqlQuery q;
    q.prepare(SELECT_GENRE_SEXE);
    q.addBindValue(sexe);
    if(q.exec()){
        if(!q.next())
        {
           q.prepare(INSERT_GENRE_SQL);
           q.addBindValue(sexe);
           q.exec();
        }
    }
}

void removeParticipant(const int id)
{
    QSqlQuery q;
    q.prepare(DELETE_PARTICIPANT_SQL);
    q.addBindValue(id);
    q.exec();
}


QSqlError initDb()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("course.db");

    if (!db.open())
        return db.lastError();

    QStringList tables = db.tables();
    if (tables.contains("mail", Qt::CaseInsensitive)
        && tables.contains("nom", Qt::CaseInsensitive))
        return QSqlError();

    QSqlQuery q;
    if (!q.exec(PARTICIPANTS_SQL))
        return q.lastError();
    if (!q.exec(GENRES_SQL))
        return q.lastError();

    insertGenreIfNotExist(q, QString("masculin"));
    insertGenreIfNotExist(q, QString("feminin"));

    //addParticipant(q, QString("Khazri"), QString("Walid"), QString("test1@gmail.com"), QString::fromLocal8Bit(QCryptographicHash::hash("mdp123", QCryptographicHash::Sha256)), QDate::currentDate().toString("dd/MM/yyyy"), 1);
    //addParticipant(q, QString("Khazri2"), QString("Walid2"), QString("test133@gmail.com"), QString::fromLocal8Bit(QCryptographicHash::hash("mdp12399", QCryptographicHash::Sha256)), QDate::currentDate().toString("dd/MM/yyyy"), 1);


    return QSqlError();
}

#endif // DATABASE_MANAGER_H
