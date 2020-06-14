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
public:
    virtual ~BackupJob() = default;
    /** @var Path to files to back up. */
    std::string m_SourcePath;
    /** @var Path to where to save the backup. */
    std::string m_DestinationPath;
    std::string m_Name;
    bool        m_Compressed;
    /** @var ID of backup from config if saved. */
    int64_t m_ID;
    long long m_LastCompleted;

    BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id = -1,
               long long lastFinished = -1);
    /**
     * Start backup of this job.
     */

    int Backup (UserInterface* ui, bool disableTimeComp);
    int Restore (UserInterface* ui, int64_t snapshotId, std::string filePath);

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
