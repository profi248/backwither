#ifndef BACKUP_BACKUP_H
#define BACKUP_BACKUP_H
#include "BackupJob.h"
#include "ConfigProvider.h"

/**
 * Main class for backup application.
 */
class Backup {
public:
    static int DoBackup (BackupJob* job, ConfigProvider* config);
private:
    static std::string normalizeDirectoryPath (std::string path);
    static bool        isDirectoryEmpty (std::string path);
    static void        verifySourceDirectory (std::string source);
    static void        verifyOrCreateDestinationDirectory (std::string destination);
};


#endif //BACKUP_BACKUP_H
