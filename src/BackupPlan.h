#ifndef BACKUP_BACKUPPLAN_H
#define BACKUP_BACKUPPLAN_H
#include <vector>
#include "BackupJob.h"

/**
 * Holds list of backup jobs.
 */
class BackupPlan {
    std::vector<BackupJob*> m_Jobs;
};


#endif //BACKUP_BACKUPPLAN_H
