#ifndef BACKWITHER_HASHFILECOMPARATOR_H
#define BACKWITHER_HASHFILECOMPARATOR_H
#include "FileComparator.h"

/**
 * Implementation of class for comparing files using hashes.
 */
class HashFileComparator : public FileComparator {
    virtual ~HashFileComparator() override ;

    /**
     * Compare files using hashes.
     * @return bool true if identical
     */
    bool CompareFiles() override;
};



#endif //BACKWITHER_HASHFILECOMPARATOR_H
