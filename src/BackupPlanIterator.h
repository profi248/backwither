#ifndef BACKWITHER_BACKUPPLANITERATOR_H
#define BACKWITHER_BACKUPPLANITERATOR_H
#include "BackupPlan.h"
#include "BackupJob.h"

class BackupPlan;

/**
 * Allow iterating over backup jobs.
 */

class BackupPlanIterator {
    BackupPlan & m_Plan;
    size_t       m_Pos;
public:
    explicit BackupPlanIterator (BackupPlan & plan);

    void Next ();
    BackupJob* GetCurrent () const;
    bool End () const;
};


#endif //BACKWITHER_BACKUPPLANITERATOR_H
