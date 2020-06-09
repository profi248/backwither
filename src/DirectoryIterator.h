#ifndef BACKWITHER_DIRECTORYITERATOR_H
#define BACKWITHER_DIRECTORYITERATOR_H
#include <set>
#include "Directory.h"
#include "SimpleIterator.h"

/**
 * Allow iterating over items in directory.
 */

class Directory;

class DirectoryIterator : public SimpleIterator {
    Directory*                                            m_Directory;
    std::set<std::shared_ptr<FilesystemEntity>>::iterator m_DirectoryIt;
public:
    explicit DirectoryIterator (Directory * dir);
    ~DirectoryIterator () override = default;

    void        Next () override;
    void        Rewind () override;
    std::string GetPath () const;
    int64_t     GetMtime () const;
    int64_t     GetSize () const;
    int64_t     GetID () const;
    void        SetID (int64_t id);
    bool        End () const override;
    bool        Empty () const override;

    std::vector<std::string> TableHeader () const override;
    std::vector<std::string> TableRow () const override;

    void operator ++ (int) override;
};


#endif //BACKWITHER_DIRECTORYITERATOR_H
