#include "dbworker.h"

DatabaseWorker::DatabaseWorker(IDatabaseAccess *db, QObject *parent)
    : QThreadWorker(parent)
    , m_db(db)
{
}

void DatabaseWorker::queryAllParticipants()
{
    m_currentQuery = LoadParticipants;
    start();
}

void DatabaseWorker::addParticipant(const Participant &p)
{
    m_pendingParticipant = p;
    m_currentQuery = AddParticipant;
    start();
}

void DatabaseWorker::updateParticipant(const Participant &p)
{
    m_pendingParticipant = p;
    m_currentQuery = UpdateParticipant;
    start();
}

void DatabaseWorker::doWork()
{
    switch (m_currentQuery) {
    case LoadParticipants: {
        QList<Participant> participants = m_db->getAllParticipants();
        emit participantsLoaded(participants);
        break;
    }
    case AddParticipant: {
        if (m_db->addParticipant(m_pendingParticipant))
            emit participantAdded(m_pendingParticipant);
        else
            emit queryFailed("Échec de l'ajout du participant");
        break;
    }
    case UpdateParticipant: {
        if (!m_db->updateParticipant(m_pendingParticipant))
            emit queryFailed("Échec de la mise à jour du participant");
        break;
    }
    }
}
