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

    /**
     * Compare two directory states by modified ttmes from filesystem
     * and return files to backup (modified since last and completely new).
     *
     * @param prev Previous directory state.
     * @param curr Current directory state.
     * @return Directory with files to backup.
     */
    Directory CompareDirs(Directory prev, Directory curr) override;
};


#endif //BACKWITHER_TIMEDIRECTORYCOMPARATOR_H
