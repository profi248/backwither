#ifndef BACKUP_CONFIGPROVIDER_H
#define BACKUP_CONFIGPROVIDER_H
#include "BackupPlan.h"
/**
 * Provides access to saving and loading backup configuration.
 */
class ConfigProvider {
public:
    ConfigProvider();
    virtual ~ConfigProvider() = default;
    /**
     * Saves provided backup plan to config storage.
     * @param plan Backup plan.
     */
    virtual void SaveBackupPlan (BackupPlan plan) = 0;
    /**
     * Retrieves a backup plan from config storage.
     * @return Backup plan.
     */
    virtual BackupPlan LoadBackupPlan () = 0;
};


#endif //BACKUP_CONFIGPROVIDER_H
