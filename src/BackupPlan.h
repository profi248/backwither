#ifndef BACKUP_BACKUPPLAN_H
#define BACKUP_BACKUPPLAN_H
#include <vector>
#include "BackupJob.h"
#include "BackupPlanIterator.h"

class BackupJob;
/**
 * Holds list of backup jobs.
 */
class BackupPlan {
    std::vector<BackupJob*> m_Jobs;
public:
    ~BackupPlan ();
    /**
     * Adds a backup job to backup plan.
     * @param job Backup job to add.
     */
    void AddBackup (BackupJob* job);

    friend class BackupPlanIterator;
};


#endif //BACKUP_BACKUPPLAN_H
