#ifndef PARTICIPANT_H
#define PARTICIPANT_H

#include <QString>
#include <QDateTime>

struct Participant {
    int      id    = -1;
    QString  lastName;
    QString  firstName;
    QString  email;
    QString  password;
    QString  year;
    int      genderId = -1;
    QDateTime registrationDate;
    bool     isActive = true;

    bool    isValid() const;
    QString getFullName() const;
};

inline bool Participant::isValid() const
{
    return !lastName.isEmpty() && !firstName.isEmpty() && !email.isEmpty();
}

inline QString Participant::getFullName() const
{
    return firstName + " " + lastName;
}

#endif // PARTICIPANT_H
