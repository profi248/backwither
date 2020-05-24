#ifndef BACKUP_BACKUP_H
#define BACKUP_BACKUP_H
#include "BackupJob.h"

/**
 * Main class for backup application.
 */
class Backup {
public:
    static int DoBackup (BackupJob* job);
private:
    static std::string normalizeDirectoryPath (std::string path);
    static bool        isDirectoryEmpty (std::string path);
};


#endif //BACKUP_BACKUP_H
