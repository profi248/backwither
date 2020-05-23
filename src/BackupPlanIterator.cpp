#include "BackupPlanIterator.h"

BackupPlanIterator::BackupPlanIterator (BackupPlan * plan) :
        m_Plan (plan),
        m_Pos (0) {}


BackupPlanIterator::~BackupPlanIterator () {
    // todo also broken...
    // delete m_Plan;
}


void BackupPlanIterator::Next () {
    m_Pos++;
}

bool BackupPlanIterator::End () const {
    return m_Pos >= m_Plan->m_Jobs.size();
}

bool BackupPlanIterator::Empty () const {
    return m_Plan->m_Jobs.empty();
}

std::string BackupPlanIterator::GetSource () const {
    return m_Plan->m_Jobs[m_Pos]->m_SourcePath;
}

std::string BackupPlanIterator::GetDestination () const {
    return m_Plan->m_Jobs[m_Pos]->m_DestinationPath;
}

std::string BackupPlanIterator::GetName () const {
    return m_Plan->m_Jobs[m_Pos]->m_Name;
}

bool BackupPlanIterator::GetIncremental () const {
    return m_Plan->m_Jobs[m_Pos]->m_Incremental;
}

BackupPlanIterator BackupPlanIterator::operator ++ (int) {
    Next();
    return *this;
}