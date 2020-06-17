#ifndef BACKUP_SQLITECONFIGPROVIDER_H
#define BACKUP_SQLITECONFIGPROVIDER_H
#include <sqlite3.h>
#include "ConfigProvider.h"
#include "ChunkList.h"

/**
 * Implements config provider as a SQLite database.
 */

class SQLiteConfigProvider : public ConfigProvider {
    // helper for sqlite3_prepare to indicate that SQL query is ended by null
    const int   SQLITE_NULL_TERMINATED   = -1;

    // path to database
    std::string m_Path;
    // store sqlite3 database object
    sqlite3*    m_DB;
public:
    /**
     * Folder under CONFIG_DEFAULT_PATH to store configuration.
     */
    const char* CONFIG_FOLDER_FILENAME   = "backwither/";

    /**
     * Name of database with list of backups and other info.
     */
    const char* CONFIG_DATABASE_FILENAME = "config.db";

    /**
     * Path under HOME where config folder is created.
     */
    const char* CONFIG_DEFAULT_PATH      = ".config/";

    /**
     * Environment variable with home directory (where config is created).
     */
    const char* CONFIG_HOME_ENVVAR       = "HOME";

    /**
     * Environment variable first searched for config location.
     */
    const char* CONFIG_DEFAULT_ENVVAR    = "XDG_CONFIG_HOME";

    /**
     * Construct config provider.
     * @param path Optional path to config folder if overridden with -c parameter.
     */
    explicit SQLiteConfigProvider (std::string path = "");
    ~SQLiteConfigProvider () override;

    void        AddBackupJob (BackupJob* job) override;
    void        DeleteBackupJob (BackupJob* job) override;
    BackupJob*  GetBackupJob (std::string name) override;
    BackupPlan* LoadBackupPlan () override;

private:
    // helper to reduce boilerplate when preparing
    int         prepareOne(const char* sql, sqlite3_stmt** stmt);
    // find config path using constants defined in this class
    std::string getDefaultConfigPath () const;
    // get path of DB in config path
    std::string getDbPath () const;
    // check if valid DB exists
    bool        configExists ();
    // create DB schema
    bool        initConfig ();
    // open DB from constructor
    sqlite3*    openDB ();
};

#endif //BACKUP_SQLITECONFIGPROVIDER_H
