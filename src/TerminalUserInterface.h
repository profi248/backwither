#ifndef BACKUP_TERMINALUSERINTERFACE_H
#define BACKUP_TERMINALUSERINTERFACE_H
#include "UserInterface.h"
#include "BackupJob.h"
#include "ConfigProvider.h"
#include "SimpleIterator.h"
/**
 * Implements user interface as terminal applicatiopn.
 */

class TerminalUserInterface : public UserInterface {
    int         m_LastStatusLength = 0;
    std::string m_ConfigPath;
public:
    const bool ENABLE_COLORS   = true;
    const bool ENABLE_PROGRESS = true;
    ~TerminalUserInterface () override = default;
    int  StartInterface (int argc, char** argv) override;
    void UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) override;
    // void ShowBackupList() override;
    // BackupJob AddBackupJob() override;
private:
    int             list ();
    int             history (char* backupName);
    int             help ();
    int             add (char* source, char* destination, char* name, bool compress);
    int             backup (char* name);
    int             restore (char* name, int64_t snapshotId, char* filePath);
    int             diff (char* name, int64_t snapshotIdA, int64_t snapshotIdB);
    int             show (char* name, int64_t snapshotId);
    int             runCron ();
    std::string     getVersion ();
    std::string     format (std::string in, int & formatChars, bool bold = true, int color = -1);
    void            printTable (SimpleIterator* it);
    size_t          countUtf8Codepoints (std::string in);
    bool            yesNoPrompt ();
    BackupJob*      findBackupJobByName (char* name);
    BackupPlan*     loadBackupPlan ();
    ConfigProvider* getConfigProvider () const;
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
