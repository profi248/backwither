#ifndef BACKWITHER_BACKUPPLANITERATOR_H
#define BACKWITHER_BACKUPPLANITERATOR_H
#include "BackupPlan.h"
#include "BackupJob.h"

class BackupPlan;

/**
 * Allow iterating over backup jobs.
 */

class BackupPlanIterator {
    BackupPlan * m_Plan;
    size_t       m_Pos;
public:
    explicit BackupPlanIterator (BackupPlan * plan);
    ~BackupPlanIterator ();

    void Next ();
    std::string GetSource () const;
    std::string GetDestination () const;
    std::string GetName () const;
    bool GetIncremental () const;
    bool End () const;
    bool Empty () const;

    BackupPlanIterator operator ++ (int);
};


#endif //BACKWITHER_BACKUPPLANITERATOR_H