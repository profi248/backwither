#include "SnapshotListIterator.h"
#include "../Backup/TimeUtils.h"

SnapshotListIterator::SnapshotListIterator (SnapshotList * list) :
        m_SnapshotList (list),
        m_Pos (0) {}

void SnapshotListIterator::Next () {
    m_Pos++;
}

void SnapshotListIterator::Rewind () {
    m_Pos = 0;
}

bool SnapshotListIterator::End () const {
    return m_Pos >= m_SnapshotList->m_Snapshots.size();
}

bool SnapshotListIterator::Empty () const {
    return m_SnapshotList->m_Snapshots.empty();
}

int64_t SnapshotListIterator::GetID () const {
    return m_SnapshotList->m_Snapshots[m_Pos].GetID();
}

long long SnapshotListIterator::GetCreation() const {
    return m_SnapshotList->m_Snapshots[m_Pos].GetCreation();
}


long long SnapshotListIterator::GetCompletion () const {
    return m_SnapshotList->m_Snapshots[m_Pos].GetCompletion();
}


void SnapshotListIterator::operator ++ (int) {
    Next();
}

std::vector<std::string> SnapshotListIterator::TableHeader () const {
    return { "ID", "started", "completed" };
}

std::vector<std::string> SnapshotListIterator::TableRow () const {
        return { std::to_string(GetID()),
                 TimeUtils::HumanDateTime(GetCreation()),
                 TimeUtils::HumanDateTime(GetCompletion()) };
}
