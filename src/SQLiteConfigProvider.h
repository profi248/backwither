#ifndef BACKUP_SQLITECONFIGPROVIDER_H
#define BACKUP_SQLITECONFIGPROVIDER_H
#include <sqlite3.h>
#include "ConfigProvider.h"
#include "ChunkList.h"

/**
 * Implements config provider as a SQLite database.
 */

class SQLiteConfigProvider : public ConfigProvider {
    const int   SQLITE_NULL_TERMINATED   = -1;

    std::string m_Path;
    sqlite3*    m_DB;

public:
    const char* CONFIG_FOLDER_FILENAME   = "backwither/";
    const char* CONFIG_DATABASE_FILENAME = "config.db";
    const char* CONFIG_DEFAULT_PATH      = ".config/";
    const char* CONFIG_HOME_ENVVAR       = "HOME";
    const char* CONFIG_DEFAULT_ENVVAR    = "XDG_CONFIG_HOME";

    explicit SQLiteConfigProvider (std::string path = "");
    ~SQLiteConfigProvider () override;

    void        AddBackupJob (BackupJob* job) override;
    BackupJob*  GetBackupJob (std::string name) override;
    BackupPlan* LoadBackupPlan () override;

private:
    std::string getDefaultConfigPath () const;
    std::string getDbPath () const;
    bool        configExists ();
    bool        initConfig ();
    sqlite3*    openDB ();
};

#endif //BACKUP_SQLITECONFIGPROVIDER_H
