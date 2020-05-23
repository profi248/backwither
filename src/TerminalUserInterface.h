#ifndef BACKUP_TERMINALUSERINTERFACE_H
#define BACKUP_TERMINALUSERINTERFACE_H
#include "UserInterface.h"
#include "BackupJob.h"
#include "ConfigProvider.h"

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
    int         add (char* source, char* destination, char* name, char* configPath);

    ConfigProvider* getConfigProvider (const char* configPath) const;
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
