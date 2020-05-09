#ifndef BACKUP_USERINTERFACE_H
#define BACKUP_USERINTERFACE_H
#include "BackupJob.h"

/**
 * Provides user interface for backup management.
 */

class UserInterface {
    virtual void ShowBackupList();
    virtual BackupJob AddBackupJob();
};


#endif //BACKUP_USERINTERFACE_H
