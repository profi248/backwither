#ifndef BACKWITHER_FILECOMPARATOR_H
#define BACKWITHER_FILECOMPARATOR_H

/**
 * Class for comparing files.
 */
class FileComparator {
    virtual ~FileComparator();

    /**
     * Compare files.
     * @return bool true if identical
     */
    virtual bool CompareFiles() = 0;
};


#endif //BACKWITHER_FILECOMPARATOR_H
