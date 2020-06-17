#ifndef BACKUP_CONFIGPROVIDER_H
#define BACKUP_CONFIGPROVIDER_H
#include "BackupPlan.h"
#include "../Filesystem/Directory.h"
#include "BackupJob.h"
#include "ChunkList.h"

/**
 * Base class for config provider (list of backups and other settings).
 */

class BackupJob;
class BackupPlan;

/**
 * Provides access to saving and loading backup configuration.
 */
class ConfigProvider {
public:
    ConfigProvider() = default;
    virtual ~ConfigProvider() = default;

    /**
     * Adds new backup job to plan in config storage.
     * @param job Backup job.
     */
    virtual void AddBackupJob (BackupJob* job) = 0;
    /**
     * Get a backup job from config storage by name.
     * @param name Name of the backup job.
     * @return The backub job.
     */
    virtual BackupJob* GetBackupJob (std::string name) = 0;
    /**
     * Delete specified backup job from storage.
     * @param job The backup job to delete.
     */
    virtual void DeleteBackupJob (BackupJob* job) = 0;
    /**
     * Retrieves a backup plan from config storage.
     * @return Backup plan.
     */
    virtual BackupPlan* LoadBackupPlan () = 0;

};


#endif //BACKUP_CONFIGPROVIDER_H
