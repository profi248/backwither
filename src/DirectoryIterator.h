#ifndef BACKWITHER_DIRECTORYITERATOR_H
#define BACKWITHER_DIRECTORYITERATOR_H
#include <set>
#include "Directory.h"

/**
 * Allow iterating over items in directory.
 */

class Directory;

class DirectoryIterator {
    Directory*                                            m_Directory;
    std::set<std::shared_ptr<FilesystemEntity>>::iterator m_DirectoryIt;
public:
    explicit DirectoryIterator (Directory * dir);
    ~DirectoryIterator () = default;

    void Next ();
    std::string GetPath () const;
    int64_t     GetMtime () const;
    int64_t     GetSize () const;
    bool End () const;
    bool Empty () const;

    DirectoryIterator operator ++ (int);
};


#endif //BACKWITHER_DIRECTORYITERATOR_H
