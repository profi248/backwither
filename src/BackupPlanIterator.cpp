#include "BackupPlanIterator.h"
#include "TimeUtils.h"

BackupPlanIterator::BackupPlanIterator (BackupPlan * plan) :
        m_Plan (plan),
        m_Pos (0) {}

void BackupPlanIterator::Next () {
    m_Pos++;
}


void BackupPlanIterator::Rewind () {
    m_Pos = 0;
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

bool BackupPlanIterator::IsCompressed () const {
    return m_Plan->m_Jobs[m_Pos]->m_Compressed;
}

void BackupPlanIterator::operator ++ (int) {
    Next();
}

std::vector<std::string> BackupPlanIterator::TableRow () const {
    return { GetName(), GetSource(), GetDestination(),
             IsCompressed() ? "yes" : "no",
             TimeUtils::HumanDateTime(m_Plan->m_Jobs[m_Pos]->m_LastCompleted) };
}

std::vector<std::string> BackupPlanIterator::TableHeader () const {
    return { "name", "source", "destination", "compressed", "last successful run" };
}
