#include "databasemanager.h"


DatabaseManager::DatabaseManager()
{
}

QSqlError DatabaseManager::initDb()
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

    insertGenreIfNotExist(QString("masculin"));
    insertGenreIfNotExist(QString("feminin"));
    insertGenreIfNotExist(QString("Non binaire"));

    this->addParticipant(QString("Khazri"), QString("Walid"), QString("test1@gmail.com"), QString::fromLocal8Bit(QCryptographicHash::hash("mdp123", QCryptographicHash::Sha256)), QDate::currentDate().toString("dd/MM/yyyy"), 1);
    //addParticipant(q, QString("Khazri2"), QString("Walid2"), QString("test133@gmail.com"), QString::fromLocal8Bit(QCryptographicHash::hash("mdp12399", QCryptographicHash::Sha256)), QDate::currentDate().toString("dd/MM/yyyy"), 1);


    return QSqlError();
}


void DatabaseManager::addParticipant(const QString &lastname, const QString &firstname, const QString &mail, const QString &password, const QString &date_birthday, int genreId)
{
    QSqlQuery q;
    q.prepare(INSERT_PARTICIPANT_SQL);
    q.addBindValue(lastname);
    q.addBindValue(firstname);
    q.addBindValue(mail);
    q.addBindValue(password);
    q.addBindValue(date_birthday);
    q.addBindValue(genreId);
    q.exec();
}

void DatabaseManager::insertGenreIfNotExist(const QString &sexe)
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

void DatabaseManager::removeParticipant(const int id)
{
    QSqlQuery q;
    q.prepare(DELETE_PARTICIPANT_SQL);
    q.addBindValue(id);
    q.exec();
}

