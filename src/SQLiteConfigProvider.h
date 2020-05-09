#ifndef BACKUP_SQLITECONFIGPROVIDER_H
#define BACKUP_SQLITECONFIGPROVIDER_H
#include "ConfigProvider.h"

/**
 * Implements config provider as a SQLite database.
 */

class SQLiteConfigProvider : public ConfigProvider {
    SQLiteConfigProvider();
    ~SQLiteConfigProvider() override;
    void SaveBackupPlan (BackupPlan plan) override;
    BackupPlan LoadBackupPlan () override;
};

#endif //BACKUP_SQLITECONFIGPROVIDER_H
