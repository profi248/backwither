#include "Snapshot.h"

Snapshot::Snapshot (int64_t id, long long int created) :
    m_ID (id),
    m_Created (created) {}

int64_t Snapshot::GetID () const {
    return m_ID;
}

long long Snapshot::GetCreation () const {
    return m_Created;
}
