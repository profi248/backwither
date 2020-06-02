#ifndef BACKUP_BACKUPJOB_H
#define BACKUP_BACKUPJOB_H
#include <string>
#include "BackupPlanIterator.h"
#include "ConfigProvider.h"
#include "UserInterface.h"

class BackupPlanIterator;
class ConfigProvider;
class UserInterface;

/**
 * Represents a backup job for a certain filesystem path.
 */
class BackupJob {
    /** @var Path to files to back up. */
    std::string m_SourcePath;
    /** @var Path to where to save the backup. */
    std::string m_DestinationPath;
    std::string m_Name;
    bool        m_Compressed;
    /** @var ID of backup from config if saved. */
    int64_t m_ID;
public:
    BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id = -1) ;
    /**
     * Start backup of this job.
     */

    int Backup  (ConfigProvider* config, UserInterface* ui);
    int Restore (ConfigProvider* config, UserInterface* ui, int64_t snapshotId = 0);

    std::string GetSource () const;
    std::string GetDestination () const;
    std::string GetName () const;
    bool        IsCompressed () const;
    int64_t     GetID () const;

    friend BackupPlanIterator;

};


#endif //BACKUP_BACKUPJOB_H
