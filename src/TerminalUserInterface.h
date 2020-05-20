#ifndef BACKUP_TERMINALUSERINTERFACE_H
#define BACKUP_TERMINALUSERINTERFACE_H
#include "UserInterface.h"
#include "BackupJob.h"

/**
 * Implements user interface as terminal applicatiopn.
 */

class TerminalUserInterface : public UserInterface {
public:
    ~TerminalUserInterface () override = default;
    int StartInterface (int argc, char** argv) override;
    // void ShowBackupList() override;
    // BackupJob AddBackupJob() override;
private:
    int         list (char* configPath);
    int         help ();
    std::string getVersion ();
    bool        add (char* path, char* name);
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
