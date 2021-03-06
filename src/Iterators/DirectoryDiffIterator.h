#ifndef BACKWITHER_DIRECTORYDIFFITERATOR_H
#define BACKWITHER_DIRECTORYDIFFITERATOR_H

#include "SimpleIterator.h"
#include "../Filesystem/Directory.h"

class Directory;

/**
 * Class for iterating over *modified* files between snapshots.
 * Calculates size differences and shows file name with modification times.
 */

class DirectoryDiffIterator : public SimpleIterator {
    Directory*                                                                       m_DirA;
    Directory*                                                                       m_DirB;
    std::set<std::shared_ptr<FilesystemEntity>, FilesystemEntity::Compare>::iterator m_DirAIt;
    std::set<std::shared_ptr<FilesystemEntity>, FilesystemEntity::Compare>::iterator m_DirBIt;
public:
    DirectoryDiffIterator (Directory * a, Directory * b);
    ~DirectoryDiffIterator () override = default;

    void Next () override;
    void Rewind () override;
    bool End () const override;
    bool Empty () const override;

    std::vector<std::string> TableHeader () const override;
    std::vector<std::string> TableRow () const override;

    void operator ++ (int) override;
private:
    void init ();
};


#endif //BACKWITHER_DIRECTORYDIFFITERATOR_H
