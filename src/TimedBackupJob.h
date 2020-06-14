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

    /**
     * Returns formatted string with backup plan.
     * @return Human friendly backup plan string.
     */
    std::string GetPlan () const override;

    /**
     * Get weekday_t of planned backup.
     * @return Planned weekday.
     */
    TimeUtils::weekday_t GetPlanWeekday () const override;

    /**
     * Get seconds since day started of planned backup.
     * @return Planned backup time.
     */
    int GetPlanSecsSinceDay () const override;
};


#endif //BACKUP_TIMEDBACKUPJOB_H
