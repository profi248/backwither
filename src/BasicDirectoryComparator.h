#ifndef BACKWITHER_BASICDIRECTORYCOMPARATOR_H
#define BACKWITHER_BASICDIRECTORYCOMPARATOR_H
#include "DirectoryComparator.h"
/**
 * Trivial implemenation of class for comparing directories, simply return current state.
 */
class BasicDirectoryComparator : public DirectoryComparator {
    virtual ~BasicDirectoryComparator() = default;

    virtual Directory CompareDirs(Directory prev, Directory curr) {
        return curr;
    };
};


#endif //BACKWITHER_BASICDIRECTORYCOMPARATOR_H
