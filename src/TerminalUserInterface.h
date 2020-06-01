#ifndef BACKUP_TERMINALUSERINTERFACE_H
#define BACKUP_TERMINALUSERINTERFACE_H
#include "UserInterface.h"
#include "BackupJob.h"
#include "ConfigProvider.h"

/**
 * Implements user interface as terminal applicatiopn.
 */

class TerminalUserInterface : public UserInterface {
    int m_LastStatusLength = 0;
public:
    ~TerminalUserInterface () override = default;
    int StartInterface (int argc, char** argv) override;
    void UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) override;
    // void ShowBackupList() override;
    // BackupJob AddBackupJob() override;
private:
    int         list (char* configPath);
    int         help ();
    std::string getVersion ();
    int         add (char* source, char* destination, char* name, char* configPath);
    int         run (char* name, char* configPath);
    int         restore (char* name, char* configPath);
    std::string humanFileSize (size_t bytes);

    ConfigProvider* getConfigProvider (const char* configPath) const;
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
