#ifndef BACKUP_SQLITECONFIGPROVIDER_H
#define BACKUP_SQLITECONFIGPROVIDER_H
#include <sqlite3.h>
#include "ConfigProvider.h"

/**
 * Implements config provider as a SQLite database.
 */

class SQLiteConfigProvider : public ConfigProvider {
    std::string m_Path;
public:
    explicit SQLiteConfigProvider (std::string path = "");

    ~SQLiteConfigProvider () override = default;

    void AddBackupJob (BackupJob* job) override;

    BackupJob* GetBackupJob (std::string name) override;

    BackupPlan* LoadBackupPlan () override;

    void SaveSnapshotFileIndex (Directory fld, BackupJob *job) override;

    Directory LoadSnapshotFileIndex (BackupJob* job, int64_t snapshotID) override;

private:
    std::string getDefaultConfigPath () const;

    std::string getDbPath () const;

    bool        configExists ();

    bool        initConfig ();

    sqlite3*    openDB();

    int64_t getLastSnapshotId (const BackupJob* job);
};

#endif //BACKUP_SQLITECONFIGPROVIDER_H
