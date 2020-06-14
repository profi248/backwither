#ifndef BACKUP_BACKUPJOB_H
#define BACKUP_BACKUPJOB_H
#include <string>
#include "BackupPlanIterator.h"
#include "ConfigProvider.h"
#include "UserInterface.h"
#include "TimeUtils.h"

class BackupPlanIterator;
class ConfigProvider;
class UserInterface;

/**
 * Represents a backup job for a certain filesystem path.
 */
class BackupJob {
    std::string m_SourcePath;
    std::string m_DestinationPath;
    std::string m_Name;
    bool        m_Compressed;
    int64_t m_ID;
public:
    // todo make private
    long long m_LastCompleted;
    virtual ~BackupJob() = default;
    /**
     * Construct a BackupJob object.
     *
     * @param source Absolute path to backup source directory.
     * @param destination Absolute path to backup destination directory.
     * @param name Backup name (unique).
     * @param compressed Whether to compress backup.
     * @param id Backup ID if known, -1 otherwise.
     * @param lastFinished Unix timestamp of last backup run if known, -1 otherwise.
     */
    BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id = -1,
               long long lastFinished = -1);

    /**
     * Do backup of the job.
     *
     * @param ui UserInterface pointer where to send progress, nullptr otherwise.
     * @param disableTimeComp True to disable smart change detection by filesystem modified time for this run.
     */
    void Backup (UserInterface* ui, bool disableTimeComp);

    /**
     * Do restoration of a specific snapshot (optionally for single file).
     * @param ui UserInterface pointer where to send progress, nullptr otherwise.
     * @param snapshotId Snapshot ID, 0 for latest.
     * @param filePath Optionally a relative path to a single file to restore.
     */
    void Restore (UserInterface* ui, int64_t snapshotId, std::string filePath);

    std::string                  GetSource () const;
    std::string                  GetDestination () const;
    std::string                  GetName () const;
    virtual std::string          GetPlan () const;
    virtual bool                 ShouldStartBackup () const;
    virtual TimeUtils::weekday_t GetPlanWeekday () const;
    virtual int GetPlanSecsSinceDay () const;
    bool                         IsCompressed () const;
    int64_t                      GetID () const;

    friend BackupPlanIterator;

};


#endif //BACKUP_BACKUPJOB_H
