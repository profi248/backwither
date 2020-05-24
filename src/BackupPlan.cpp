#include "BackupPlan.h"

BackupPlan::~BackupPlan () {
    // todo broken... wtf
    // maybe look at history for PT6 how I solved the issue there
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