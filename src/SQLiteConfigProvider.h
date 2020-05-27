#ifndef BACKUP_SQLITECONFIGPROVIDER_H
#define BACKUP_SQLITECONFIGPROVIDER_H
#include <sqlite3.h>
#include "ConfigProvider.h"

/**
 * Implements config provider as a SQLite database.
 */

class SQLiteConfigProvider : public ConfigProvider {
    const char* CONFIG_FOLDER_FILENAME   = "backwither/";
    const char* CONFIG_DATABASE_FILENAME = "config.db";
    const char* CONFIG_DEFAULT_PATH      = ".config/";
    const char* CONFIG_HOME_ENVVAR       = "HOME";
    const char* CONFIG_DEFAULT_ENVVAR    = "XDG_CONFIG_HOME";
    const int   SQLITE_NULL_TERMINATED = -1;

    std::string m_Path;
    sqlite3*    m_DB;

public:
    explicit SQLiteConfigProvider (std::string path = "");
    ~SQLiteConfigProvider () override;

    void        AddBackupJob (BackupJob* job) override;
    BackupJob*  GetBackupJob (std::string name) override;
    BackupPlan* LoadBackupPlan () override;
    void        SaveSnapshotFileIndex (Directory fld, BackupJob *job) override;
    Directory   LoadSnapshotFileIndex (BackupJob* job, int64_t snapshotID) override;

private:
    std::string getDefaultConfigPath () const;
    std::string getDbPath () const;
    bool        configExists ();
    bool        initConfig ();
    sqlite3*    openDB();
    int64_t     getLastSnapshotId (const BackupJob* job);
};

#endif //BACKUP_SQLITECONFIGPROVIDER_H
