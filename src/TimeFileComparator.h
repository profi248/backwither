#ifndef BACKWITHER_TIMEFILECOMPARATOR_H
#define BACKWITHER_TIMEFILECOMPARATOR_H
#include "FileComparator.h"

/**
 * Implementation of class for comparing files using time info from filesystem.
 */
class TimeFileComparator : public FileComparator {
public:
    TimeFileComparator();
    ~TimeFileComparator() override = default;

    Directory CompareDirs(Directory prev, Directory curr) override;
};


#endif //BACKWITHER_TIMEFILECOMPARATOR_H
