#include "Snapshot.h"

Snapshot::Snapshot (int64_t id, long long created, long long completed) :
    m_ID (id),
    m_Created (created),
    m_Completed (completed) {}

int64_t Snapshot::GetID () const {
    return m_ID;
}

long long Snapshot::GetCreation () const {
    return m_Created;
}

long long Snapshot::GetCompletion () const {
    return m_Completed;
}
