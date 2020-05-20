#include "BackupPlanIterator.h"

BackupPlanIterator::BackupPlanIterator (BackupPlan & plan) :
        m_Plan (plan),
        m_Pos (0) {}

void BackupPlanIterator::Next () {
    m_Pos++;
}

BackupJob* BackupPlanIterator::GetCurrent () const {
    return m_Plan.m_Jobs[m_Pos];
}

bool BackupPlanIterator::End () const {
    return m_Pos >= m_Plan.m_Jobs.size();
}


