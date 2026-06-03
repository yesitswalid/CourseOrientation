#include "racemanager.h"

RaceManager *RaceManager::instance = nullptr;

RaceManager *RaceManager::getInstance()
{
    if (!instance)
        instance = new RaceManager();
    return instance;
}

RaceManager::RaceManager() {}

/* ── Sélection de course ────────────────────────────────────────────────── */

void RaceManager::setRaceSelected(bool isSelected) { raceSelected = isSelected; }
bool RaceManager::isRaceSelected() const            { return raceSelected; }

/* ── Mode serveur UDP ───────────────────────────────────────────────────── */

void RaceManager::setMode(int m) { mode = m; }
int  RaceManager::getMode() const { return mode; }

/* ── Affectation groupée de tous les paramètres d'une course ────────────── */

void RaceManager::setRace(int rId, int deptId, const QString &n, const QString &d,
                          const QString &loc, float lon, float lat,
                          int diff, int t, int b)
{
    raceId       = rId;
    departmentId = deptId;
    name         = n;
    date         = d;
    location     = loc;
    gpsLongitude = lon;
    gpsLatitude  = lat;
    difficulty   = diff;
    type         = t;
    book         = b;
}

/* ── Liste des courses ──────────────────────────────────────────────────── */

void          RaceManager::setRaces(const QVector<Race> &r) { races = r; }
QVector<RaceManager::Race> RaceManager::getRaces() const    { return races; }

/* ── Accesseurs individuels ─────────────────────────────────────────────── */

int     RaceManager::getRaceId() const           { return raceId; }
void    RaceManager::setRaceId(int id)           { raceId = id; }

QString RaceManager::getName() const             { return name; }
void    RaceManager::setName(const QString &n)   { name = n; }

QString RaceManager::getDate() const             { return date; }
void    RaceManager::setDate(const QString &d)   { date = d; }

QString RaceManager::getLocation() const         { return location; }
void    RaceManager::setLocation(const QString &l) { location = l; }

int     RaceManager::getDepartmentId() const     { return departmentId; }
void    RaceManager::setDepartmentId(int id)     { departmentId = id; }

float   RaceManager::getGPSLongitude() const     { return gpsLongitude; }
void    RaceManager::setGPSLongitude(float v)    { gpsLongitude = v; }

float   RaceManager::getGPSLatitude() const      { return gpsLatitude; }
void    RaceManager::setGPSLatitude(float v)     { gpsLatitude = v; }

int     RaceManager::getDifficulty() const       { return difficulty; }
void    RaceManager::setDifficulty(int v)        { difficulty = v; }

int     RaceManager::getType() const             { return type; }
void    RaceManager::setType(int v)              { type = v; }

int     RaceManager::getBook() const             { return book; }
void    RaceManager::setBook(int v)              { book = v; }
