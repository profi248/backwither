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
    // color escape codes
    const int TERM_RED   = 31;
    const int TERM_GREEN = 32;
    const int TERM_BLUE  = 34;

    // length of last status string, so it can be cleaned up with the next one
    int         m_LastStatusLength = 0;
    // path to config if overridden
    std::string m_ConfigPath;
public:
    /**
     * Enable terminal formatting.
     */
    const bool ENABLE_COLORS   = true;

    /**
     * Enable showing progress during backup.
     */
    const bool ENABLE_PROGRESS = true;

    ~TerminalUserInterface () override = default;
    int  StartInterface (int argc, char** argv) override;
    void UpdateProgress (size_t current, size_t expected, std::string status, size_t fileSize) override;
private:
    // list all backups jobs
    int             list ();
    // history for a backup
    int             history (char* backupName);
    // show help
    int             help ();
    // add backup
    int             add (char* source, char* destination, char* name, bool compress, char* wday, char* time);
    // run backup job
    int             backup (char* name, bool disableTimeComparator);
    // restore backup job
    int             restore (char* name, int64_t snapshotId, char* filePath);
    // show difference between two snapshots of a backup job
    int             diff (char* name, int64_t snapshotIdA, int64_t snapshotIdB, char* file);
    // show files in a backup job
    int             show (char* name, int64_t snapshotId);
    // remove a backup job
    int             remove (char* name);
    // run all backup jobs
    int             runAll ();
    // run backup jobs due for backup
    int             runCron ();
    // format string to print to terminal with colors/boldness
    std::string     format (std::string in, int & formatChars, bool bold = true, int color = -1);
    // print an iterator as a pretty table
    void            printTable (SimpleIterator* it, size_t filterCol = -1, std::string filterStr = "");
    // parse user's y/n answer for confirmation
    bool            yesNoPrompt ();
    // check if snapshot ID exists in given job
    bool            verifySnapshotId (int64_t id, BackupJob* job);
    // get backup job from config by name
    BackupJob*      findBackupJobByName (char* name);
    // get backup plan from config
    BackupPlan*     loadBackupPlan ();
    // initialize config provider
    ConfigProvider* getConfigProvider () const;
};


#endif //BACKUP_TERMINALUSERINTERFACE_H
