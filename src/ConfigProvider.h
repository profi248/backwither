#ifndef BACKUP_CONFIGPROVIDER_H
#define BACKUP_CONFIGPROVIDER_H
#include "BackupPlan.h"
#include "Directory.h"
#include "BackupJob.h"
#include "ChunkList.h"

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
     * Retrieves a backup plan from config storage.
     * @return Backup plan.
     */
    virtual BackupPlan* LoadBackupPlan () = 0;

    /**
     * Save file index (root folder) to config.
     * @param fld Folder to save.
     * @return ID of newly created snapshot.
     */
    virtual int64_t SaveSnapshotFileIndex (Directory & fld, BackupJob *job) = 0;

   /**
    * Retrieve file index (root folder) from config.
    * @return Directory
    */
    virtual Directory LoadSnapshotFileIndex (BackupJob* job, int64_t snapshotID = -1) = 0;

    /**
     * Store chunks that a file comprises from (ChunkList).
     * @param chunks Chunks of a file.
     */
    virtual void SaveFileChunks (ChunkList chunks, int64_t snapshotId) = 0;

    virtual ChunkList RetrieveFileChunks (BackupJob* job, int64_t snapshotId, int64_t fileId) = 0;

};


#endif //BACKUP_CONFIGPROVIDER_H
