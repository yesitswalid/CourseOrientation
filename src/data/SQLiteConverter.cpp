#include "SQLiteConverter.h"
#include <QApplication>
#include <QDebug>
#include <QtSql>

SQLiteConverter::SQLiteConverter()
{
    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "mySqlite"));
}

SQLiteConverter::SQLiteConverter(const QString &dbPath)
{
    this->path = QApplication::applicationDirPath() + "/" + dbPath;
    m_db = new QSqlDatabase(QSqlDatabase::addDatabase("QSQLITE", "mySqlite1"));
    m_db->setDatabaseName(this->path);
    if (!m_db->open())
        qWarning() << "Erreur lors de l'ouverture du fichier :" << m_db->lastError().text();
}

SQLiteConverter::~SQLiteConverter()
{
    if (m_db) {
        m_db->close();
        delete m_db;
        m_db = nullptr;
    }
}

bool SQLiteConverter::exportSql()
{
    return true;
}

QSqlDatabase *SQLiteConverter::getDatabase()
{
    return m_db;
}

void SQLiteConverter::setPath(const QString &p)
{
    path = p;
}

QString SQLiteConverter::getPath() const
{
    return path;
}
