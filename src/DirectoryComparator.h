#ifndef BACKWITHER_DIRECTORYCOMPARATOR_H
#define BACKWITHER_DIRECTORYCOMPARATOR_H

#include "Directory.h"

/**
 * Class for comparing directories.
 */
class DirectoryComparator {
public:
    virtual ~DirectoryComparator() = default;
    /**
     * Compare two directories.
     * @return Directory with only new + modified files.
     */
    virtual Directory CompareDirs(Directory prev, Directory curr) = 0;
};


#endif //BACKWITHER_DIRECTORYCOMPARATOR_H
