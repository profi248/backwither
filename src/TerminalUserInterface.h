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
    int             list (char* configPath);
    int history (char* configPath, char* backupName);
    int             help ();
    std::string     getVersion ();
    int             add (char* source, char* destination, char* name, char* configPath, bool compress);
    int             backup (char* name, char* configPath);
    int             restore (char* name, char* configPath);
    std::string     humanFileSize (size_t bytes);
    std::string     format (std::string in, int & formatChars, bool bold = true, int color = -1);
    bool            yesNoPrompt ();
    ConfigProvider* getConfigProvider (const char* configPath) const;
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
