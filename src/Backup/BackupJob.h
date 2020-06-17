#ifndef BACKUP_BACKUPJOB_H
#define BACKUP_BACKUPJOB_H
#include <string>
#include "../Iterators/BackupPlanIterator.h"
#include "ConfigProvider.h"
#include "../UI/UserInterface.h"
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
    long long   m_LastCompleted = -1;
    int64_t     m_ID;
public:
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
    BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id = -1);

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

    /**
     * Get source path.
     * @return Path.
     */
    std::string GetSource () const;

    /**
     * Get destination path.
     * @return Path.
     */
    std::string GetDestination () const;

    /**
     * Get backup name.
     * @return Name.
     */
    std::string GetName () const;

    /**
     * Get backup completion if known, else get -1.
     * @return Unix timestamp of last completion.
     */
    long long GetLastCompleted () const;

    /**
     * Set backup completion.
     * @param timestamp Unix timestamp of last backup completion.
     */
    void SetLastCompleted (long long timestamp);

    /**
     * Get human-friendly description of backup plan.
     * @return Formatted plan string.
     */
    virtual std::string GetPlan () const;

    /**
     * Determine whether backup should be started based on last completion and computed planned run.
     * @return Should trigger backup.
     */
    virtual bool ShouldStartBackup () const;

    /**
     * Get weekday of plan.
     * @return Weekday.
     */
    virtual TimeUtils::weekday_t GetPlanWeekday () const;

    /**
     * Get seconds since start of day of plan start.
     * @return Seconds.
     */
    virtual int GetPlanSecsSinceDay () const;

    /**
     * Return whether backup is compressed or not.
     * @return Is backup compressed.
     */
    bool IsCompressed () const;

    /**
     * Return backup ID or -1 if unknown.
     * @return ID.
     */
    int64_t GetID () const;

    friend BackupPlanIterator;
};


#endif //BACKUP_BACKUPJOB_H
