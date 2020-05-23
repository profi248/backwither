#ifndef BACKUP_BACKUPJOB_H
#define BACKUP_BACKUPJOB_H
#include <string>
#include "BackupPlanIterator.h"

class BackupPlanIterator;

/**
 * Represents a backup job for a certain filesystem path.
 */
class BackupJob {
    /** @var Path to files to back up. */
    std::string m_SourcePath;
    /** @var Path to where to save the backup. */
    std::string m_DestinationPath;
    std::string m_Name;
    bool        m_Incremental;
public:
    BackupJob (std::string source, std::string destination, std::string name, bool incremental);
    /**
     * Start backup of this job.
     */
    void Backup ();

    std::string GetSource () const;
    std::string GetDestination () const;
    std::string GetName () const;
    bool        GetIncremental () const;

    friend BackupPlanIterator;
};


#endif //BACKUP_BACKUPJOB_H
