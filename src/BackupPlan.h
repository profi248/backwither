#ifndef BACKUP_BACKUPPLAN_H
#define BACKUP_BACKUPPLAN_H
#include <vector>
#include "BackupJob.h"
#include "BackupPlanIterator.h"

/**
 * Holds list of backup jobs.
 */
class BackupPlan {
    std::vector<BackupJob*> m_Jobs;
public:
    void AddBackup ();

    friend class BackupPlanIterator;
};


#endif //BACKUP_BACKUPPLAN_H
