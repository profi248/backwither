#ifndef BACKUP_TIMEDBACKUPJOB_H
#define BACKUP_TIMEDBACKUPJOB_H
#include "BackupJob.h"
#include "TimeUtils.h"
/**
 * Backup job with timed plan.
 */

class TimedBackupJob : public BackupJob {
public:
    /**
     * Consturct timed backup job.
     * @param source Absolute path to backup source directory.
     * @param destination Absolute path to backup destination directory.
     * @param name Backup name (unique).
     * @param compressed Whether to compress backup.
     * @param day Day of week to trigger backup.
     * @param time Time of day to trigger backup (saved as seconds since start of the day (3600 = 01:00).
     * @param id Backup ID if known, -1 otherwise.
     * @param lastFinished Unix timestamp of last backup run if known, -1 otherwise.
     */
    TimedBackupJob (std::string source, std::string destination, std::string name, bool compressed,
                    TimeUtils::weekday_t day, int time, int64_t id = -1);

private:
    // day of week to trigger backup
    TimeUtils::weekday_t m_PlanDayOfWeek;
    // time of day to trigger backup, saved as seconds since start of day (e.g. 3600 = 01:00)
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
