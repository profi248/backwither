#include "BackupPlan.h"

BackupPlan::~BackupPlan () {
    // todo broken... wtf
    /*
    for (auto & it : m_Jobs) {
        delete it;
    }
     */

    // m_Jobs.clear();
}

void BackupPlan::AddBackup (BackupJob* job) {
    m_Jobs.push_back(job);
}