#ifndef BACKUP_BACKUPJOB_H
#define BACKUP_BACKUPJOB_H
#include <string>

/**
 * Represents a backup job for a certain filesystem path.
 */
class BackupJob {
    std::string m_Path;
    std::string m_Name;
    bool        m_Incremental;
public:
    /**
     * Start backup of this job.
     */
    void Backup ();
};


#endif //BACKUP_BACKUPJOB_H
