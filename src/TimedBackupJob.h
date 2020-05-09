#ifndef BACKUP_TIMEDBACKUPJOB_H
#define BACKUP_TIMEDBACKUPJOB_H
#include "BackupJob.h"

/**
 * Backup job with timed plan.
 */

class TimedBackupJob : public BackupJob {
public:
    enum weekday_t {MON, TUE, WED, THU, FRI, SAT, SUN};
private:
    weekday_t m_TriggerDayOfWeek;
    int       m_TriggerSecondsSinceDayStarted;
public:
    /**
     * Tells if job is due for backup.
     * @return Whether to do backup now.
     */
    bool ShouldStartBackup ();
};


#endif //BACKUP_TIMEDBACKUPJOB_H
