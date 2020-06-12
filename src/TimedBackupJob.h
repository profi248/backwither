#ifndef BACKUP_TIMEDBACKUPJOB_H
#define BACKUP_TIMEDBACKUPJOB_H
#include "BackupJob.h"
#include "TimeUtils.h"
/**
 * Backup job with timed plan.
 */

class TimedBackupJob : public BackupJob {
public:
    TimedBackupJob (std::string source, std::string destination, std::string name, bool compressed,
                    TimeUtils::weekday_t day, int time, int64_t id = -1, long long lastFinished = -1);

private:
    TimeUtils::weekday_t m_PlanDayOfWeek;
    int                  m_PlanSecondsSinceDayStarted;
public:
    virtual ~TimedBackupJob() = default;
    /**
     * Tells if job is due for backup.
     * @return Whether to do backup now.
     */
    bool ShouldStartBackup () const override;
    std::string GetPlan () const override;
};


#endif //BACKUP_TIMEDBACKUPJOB_H
