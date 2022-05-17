#include "racemanager.h"



RaceManager* RaceManager::instance = 0;

RaceManager* RaceManager::getInstance()
{
    if (instance == 0)
    {
        instance = new RaceManager();
    }

    return instance;
}


RaceManager::RaceManager()
{
}


void RaceManager::setRaceSelected(bool isSelected)
{
    this->raceSelected = isSelected;
}

bool RaceManager::isRaceSelected()
{
    return this->raceSelected;
}

void RaceManager::setRace(int raceId, int departmentId, QString name, QString date, QString location, float gpsLongitude, float gpsLatitude, int difficulty, int type, int book)
{
    this->raceId = raceId;
    this->departmentId = departmentId;
    this->name = name;
    this->date = date;
    this->location = location;
    this->gpsLongitude = gpsLongitude;
    this->gpsLatitude = gpsLatitude;
    this->difficulty = difficulty;
    this->type = type;
    this->book = book;
}

QString RaceManager::getName()
{
    return this->name;
}

void RaceManager::setName(QString name)
{
    this->name = name;
}


int RaceManager::getRaceId()
{
    return this->raceId;
}

void RaceManager::setRaceId(int raceId)
{
    this->raceId = raceId;
}

QString RaceManager::getDate()
{
    return this->date;
}

void RaceManager::setDate(QString date)
{
    this->date = date;
}

QString RaceManager::getLocation()
{
    return this->location;
}

void RaceManager::setLocation(QString location)
{
    this->location = location;
}

int RaceManager::getDepartmentId()
{
    return this->departmentId;
}

void RaceManager::setDepartmentId(int departmentId)
{
    this->departmentId = departmentId;
}


float RaceManager::getGPSLongitude()
{
    return this->gpsLongitude;
}

void RaceManager::setGPSLongitude(float gpsLongitude)
{
    this->gpsLongitude = gpsLongitude;
}

float RaceManager::getGPSLatitude()
{
    return this->gpsLatitude;
}

void RaceManager::setGPSLatitude(float gpsLatitude)
{
    this->gpsLatitude = gpsLatitude;
}

int RaceManager::getDifficulty()
{
    return this->difficulty;
}

void RaceManager::setDifficulty(int difficulty)
{
    this->difficulty = difficulty;
}

int RaceManager::getType()
{
    return this->type;
}

void RaceManager::setType(int type)
{
    this->type = type;
}

int RaceManager::getBook()
{
    return this->book;
}

void RaceManager::setBook(int book)
{
    this->book = book;
}

void RaceManager::setRaces(QVector<RaceManager::Race> races)
{
    this->races = races;

}

QVector<RaceManager::Race> RaceManager::getRaces()
{
    return this->races;
}
