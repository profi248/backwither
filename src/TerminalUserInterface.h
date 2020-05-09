#ifndef BACKUP_TERMINALUSERINTERFACE_H
#define BACKUP_TERMINALUSERINTERFACE_H
#include "UserInterface.h"
#include "BackupJob.h"

/**
 * Implements user interface as terminal applicatiopn.
 */

class TerminalUserInterface : public UserInterface {
public:
    void ShowBackupList() override;
    BackupJob AddBackupJob() override;
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
