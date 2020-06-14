#ifndef BACKWITHER_TIMEDIRECTORYCOMPARATOR_H
#define BACKWITHER_TIMEDIRECTORYCOMPARATOR_H
#include "DirectoryComparator.h"

/**
 * Implementation of class for comparing directories using time info from filesystem.
 */
class TimeDirectoryComparator : public DirectoryComparator {
public:
    TimeDirectoryComparator() = default;
    ~TimeDirectoryComparator() override = default;

    Directory CompareDirs(Directory prev, Directory curr) override;
};


#endif //BACKWITHER_TIMEDIRECTORYCOMPARATOR_H
