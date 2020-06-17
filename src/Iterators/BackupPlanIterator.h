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

    void Next ();
    void Rewind ();

    /**
     * Get backup source.
     * @return Source path.
     */
    std::string GetSource () const;

    /**
     * Get backup destination.
     * @return Destination path.
     */
    std::string GetDestination () const;

    /**
     * Get backup name.
     * @return Backup name.
     */
    std::string GetName () const;

    /**
     * Get formatted plan string.
     * @return Backup plan string.
     */
    std::string GetPlan () const;

    /**
     * Get object on current position.
     * @return BackubJob object.
     */
    BackupJob* Current () const;

    /**
     * Check whether backup is compressed.
     * @return True for compression
     */
    bool IsCompressed () const;
    bool End () const;
    bool Empty () const;

    std::vector<std::string> TableHeader () const override;
    std::vector<std::string> TableRow () const override;
    void operator ++ (int);
};


#endif //BACKWITHER_BACKUPPLANITERATOR_H
