#ifndef BACKWITHER_FILECOMPARATOR_H
#define BACKWITHER_FILECOMPARATOR_H

#include "Directory.h"

/**
 * Class for comparing files.
 */
class FileComparator {
public:
    virtual ~FileComparator() = default;
    /**
     * Compare two directories.
     * @return Directory with only new / modified files.
     */
    virtual Directory CompareDirs(Directory prev, Directory curr) = 0;
};


#endif //BACKWITHER_FILECOMPARATOR_H
