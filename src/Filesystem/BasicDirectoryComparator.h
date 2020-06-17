#ifndef BACKWITHER_BASICDIRECTORYCOMPARATOR_H
#define BACKWITHER_BASICDIRECTORYCOMPARATOR_H
#include "DirectoryComparator.h"
/**
 * Trivial implemenation of class for comparing directories, simply return current state.
 */
class BasicDirectoryComparator : public DirectoryComparator {
    ~BasicDirectoryComparator() override = default;

    Directory CompareDirs( __attribute__((unused)) Directory prev, Directory curr) override {
        return curr;
    };
};


#endif //BACKWITHER_BASICDIRECTORYCOMPARATOR_H
