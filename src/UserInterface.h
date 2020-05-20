#ifndef BACKUP_USERINTERFACE_H
#define BACKUP_USERINTERFACE_H
#include "BackupJob.h"

/**
 * Provides user interface for backup management.
 */

class UserInterface {
public:
    virtual ~UserInterface () = default;
    virtual int StartInterface(int argc, char** argv) = 0;
    // virtual void ShowBackupList();
    // virtual BackupJob AddBackupJob();
};


#endif //BACKUP_USERINTERFACE_H
