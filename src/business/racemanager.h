#ifndef RACEMANAGER_H
#define RACEMANAGER_H

#include <QString>
#include <QVector>

/*
 * Singleton gérant l'état global de la course sélectionnée.
 *
 * Modes du serveur UDP :
 *   DEFAULT  — état initial, aucun lecteur connecté
 *   RAZ      — attribution des badges RFID aux participants avant le départ
 *   DATA     — réception des données de passage aux balises après la course
 */
class RaceManager
{
public:
    /* Identifiants des modes du serveur UDP */
    static const int DEFAULT = 0;
    static const int RAZ     = 1;
    static const int DATA    = 2;

    /* Données d'une course telles que stockées en base locale */
    struct Race {
        QString name;
        int     raceId       = -1;
        QString date;
        QString location;
        int     departmentId = -1;
        float   gpsLongitude = 0.0f;
        float   gpsLatitude  = 0.0f;
        int     difficulty   = 0;
        int     type         = 0;
        int     book         = 0;
    };

    static RaceManager *getInstance();
    RaceManager();

    /* ── Mode serveur ── */
    void setMode(int mode);
    int  getMode() const;

    /* ── Sélection de course ── */
    bool isRaceSelected() const;
    void setRaceSelected(bool isSelected);

    /* ── Liste des courses disponibles ── */
    QVector<Race> getRaces() const;
    void          setRaces(const QVector<Race> &races);

    /* ── Course sélectionnée ── */
    void setRace(int raceId, int departmentId, const QString &name,
                 const QString &date, const QString &location,
                 float gpsLongitude, float gpsLatitude,
                 int difficulty, int type, int book);

    int     getRaceId() const;
    void    setRaceId(int raceId);

    QString getName() const;
    void    setName(const QString &name);

    QString getDate() const;
    void    setDate(const QString &date);

    QString getLocation() const;
    void    setLocation(const QString &location);

    int     getDepartmentId() const;
    void    setDepartmentId(int departmentId);

    float   getGPSLongitude() const;
    void    setGPSLongitude(float gpsLongitude);

    float   getGPSLatitude() const;
    void    setGPSLatitude(float gpsLatitude);

    int     getDifficulty() const;
    void    setDifficulty(int difficulty);

    int     getType() const;
    void    setType(int type);

    int     getBook() const;
    void    setBook(int book);

protected:
    static RaceManager *instance;

private:
    bool          raceSelected = false;
    int           mode         = DEFAULT;
    QVector<Race> races;
    QString       name;
    int           raceId       = -1;
    QString       date;
    QString       location;
    int           departmentId = -1;
    float         gpsLongitude = 0.0f;
    float         gpsLatitude  = 0.0f;
    int           difficulty   = 0;
    int           type         = 0;
    int           book         = 0;
};

#endif // RACEMANAGER_H
