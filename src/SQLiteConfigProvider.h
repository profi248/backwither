#ifndef BACKUP_SQLITECONFIGPROVIDER_H
#define BACKUP_SQLITECONFIGPROVIDER_H
#include "ConfigProvider.h"

/**
 * Implements config provider as a SQLite database.
 */

class SQLiteConfigProvider : public ConfigProvider {
    std::string m_Path;
public:
    explicit SQLiteConfigProvider (std::string path = "");

    ~SQLiteConfigProvider () override = default;

    void SaveBackupPlan (BackupPlan* plan) override;

    BackupPlan* LoadBackupPlan () override;

    void SaveFileIndex (Directory fld) override;

    Directory LoadFileIndex () override;

private:
    std::string getDefaultConfigPath () const;

    std::string getDbPath () const;

    bool configExists ();

    bool initConfig ();
};

#endif //BACKUP_SQLITECONFIGPROVIDER_H
