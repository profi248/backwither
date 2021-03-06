#include "TimedBackupJob.h"
#include "TimeUtils.h"
#include "SQLiteBackupIndexProvider.h"

TimedBackupJob::TimedBackupJob (std::string source, std::string destination, std::string name, bool compressed,
                                TimeUtils::weekday_t day, int time, int64_t id)
    : BackupJob(source, destination, name, compressed, id),
      m_PlanDayOfWeek(day),
      m_PlanSecondsSinceDayStarted (time) {}

std::string TimedBackupJob::GetPlan () const {
    return TimeUtils::PlanToString(m_PlanDayOfWeek, m_PlanSecondsSinceDayStarted);
}

bool TimedBackupJob::ShouldStartBackup () const {
    BackupIndexProvider* indexProvider = new SQLiteBackupIndexProvider(const_cast<TimedBackupJob*>(this));
    long long completed = indexProvider->LastSuccessfulCompletion();
    delete indexProvider;
    return completed < TimeUtils::PlanLastScheduledTime(m_PlanDayOfWeek, m_PlanSecondsSinceDayStarted);
}

TimeUtils::weekday_t TimedBackupJob::GetPlanWeekday () const {
    return m_PlanDayOfWeek;
}

int TimedBackupJob::GetPlanSecsSinceDay () const {
    return m_PlanSecondsSinceDayStarted;
}
