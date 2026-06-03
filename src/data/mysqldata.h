#ifndef MYSQLDATA_H
#define MYSQLDATA_H

#include <QObject>
#include <QSqlDatabase>
#include <QMap>
#include <QString>

/**
 * @class MySQLData
 * @brief Gère la connexion MySQL distante et les opérations d'import/export.
 *
 * Les opérations import/export fonctionnent de manière **asynchrone** via
 * QtConcurrent : chaque appel lance un thread pool, crée ses propres
 * connexions SQL thread-locales, et émet un signal à la fin.
 * L'interface graphique ne se fige jamais.
 */
class MySQLData : public QObject
{
    Q_OBJECT

public:
    explicit MySQLData(QObject *parent = nullptr);

    MySQLData(const QString &username, const QString &password,
              const QString &hostname, const QString &dbName,
              QObject *parent = nullptr);

    ~MySQLData() override;

    /** @brief Accès à la connexion pour les tests depuis ConfigForm. */
    QSqlDatabase *getDatabase() const;

    /* ── Opérations asynchrones (ne bloquent pas l'UI) ─────────────────── */

    /** @brief Lance l'import des participants et des courses depuis le serveur MySQL.
     *  Émet importFinished(bool, QString) à la fin. */
    void importDataAsync();

    /** @brief Exporte les résultats de la course sélectionnée vers le serveur MySQL.
     *  Émet exportFinished(bool, QString) à la fin. */
    void exportDataAsync();

    /* ── Opération synchrone — uniquement pour le test de connexion ─────── */

    /** @brief Teste si la connexion MySQL est disponible. Thread-unsafe. */
    bool testConnection();

signals:
    /** @brief Émis quand l'import se termine.
     *  @param success true si l'import a réussi.
     *  @param message Message d'erreur (vide si succès). */
    void importFinished(bool success, const QString &message);

    /** @brief Émis quand l'export se termine.
     *  @param success true si l'export a réussi.
     *  @param message Message d'erreur (vide si succès). */
    void exportFinished(bool success, const QString &message);

private:
    /* Paramètres de connexion copiés pour usage dans les threads */
    struct ConnParams {
        QString username, password, hostname, dbName;
    };

    /** @brief Crée un nom de connexion unique pour un thread pool. */
    static QString threadConnName(const QString &prefix);

    /** @brief Import dans un thread dédié. Retourne un message d'erreur ou QString(). */
    static QString importThread(const ConnParams &p);

    /** @brief Export dans un thread dédié. Retourne un message d'erreur ou QString(). */
    static QString exportThread(const ConnParams &p, int raceId);

    /** @brief Déduit les ConnParams depuis l'objet m_db actuel. */
    ConnParams currentParams() const;

    ConnParams    m_params;
    QSqlDatabase *m_db = nullptr;
};

#endif // MYSQLDATA_H
