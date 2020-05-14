#ifndef BACKWITHER_TIMEFILECOMPARATOR_H
#define BACKWITHER_TIMEFILECOMPARATOR_H
#include "FileComparator.h"

/**
 * Implementation of class for comparing files using time info from filesystem.
 */
class TimeFileComparator : public FileComparator {
    virtual ~TimeFileComparator() override;

    /**
     * Compare files using time info from filesystem.
     * @return bool true if identical
     */
    bool CompareFiles() override;
};


#endif //BACKWITHER_TIMEFILECOMPARATOR_H
