#include "BackupPlan.h"

BackupPlan::~BackupPlan () {
    for (size_t i = 0; i < m_Jobs.size(); i++)
        delete m_Jobs[i];
}

void BackupPlan::AddBackup (BackupJob* job) {
    m_Jobs.push_back(job);
}