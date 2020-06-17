#ifndef BACKWITHER_BACKUPPLANITERATOR_H
#define BACKWITHER_BACKUPPLANITERATOR_H
#include "../Backup/BackupPlan.h"
#include "../Backup/BackupJob.h"
#include "SimpleIterator.h"

class BackupPlan;
class BackupJob;

/**
 * Allow iterating over backup jobs.
 */

class BackupPlanIterator : public SimpleIterator {
    BackupPlan * m_Plan;
    size_t       m_Pos;
public:
    explicit BackupPlanIterator (BackupPlan * plan);
    ~BackupPlanIterator () = default;

    void        Next ();
    void        Rewind ();
    std::string GetSource () const;
    std::string GetDestination () const;
    std::string GetName () const;
    std::string GetPlan () const;
    BackupJob*  Current () const;
    bool        IsCompressed () const;
    bool        End () const;
    bool        Empty () const;

    std::vector<std::string> TableHeader () const override;
    std::vector<std::string> TableRow () const override;
    void operator ++ (int);
};


#endif //BACKWITHER_BACKUPPLANITERATOR_H