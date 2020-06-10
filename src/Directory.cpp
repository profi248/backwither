#include <algorithm>
#include <stdexcept>
#include <iostream>
#include "Directory.h"
#include "File.h"

Directory::Directory (std::string path) {
    m_Path = path;
}

void Directory::AddFilesystemEntity (std::shared_ptr<FilesystemEntity> e) {
    m_Contents.insert(e);
}

Directory Directory::operator - (const Directory & dir) const {
    if (m_Path != dir.m_Path)
        throw std::runtime_error("Directory diff paths differ.");

    Directory diff(m_Path);

    std::set_difference(m_Contents.begin(), m_Contents.end(),
                        dir.m_Contents.begin(), dir.m_Contents.end(),
                        std::inserter(diff.m_Contents, diff.m_Contents.begin()),
                        Directory::compare);

    return diff;
}

Directory::~Directory () {
    m_Contents.clear();
}

Directory & Directory::operator = (Directory const & dir) {

    if (& dir == this)
        return *this;
    /*
    for (auto & it : m_Contents) {
        delete it;
    }

    m_Contents.clear();

    for (auto & it : dir.m_Contents) {
        m_Contents.insert(new FilesystemEntity(*it));
    }

    return *this;
     */

    return *this;
}

bool Directory::compare (const std::shared_ptr <FilesystemEntity> & a,
                         const std::shared_ptr <FilesystemEntity> & b) {
    // auto fileA = dynamic_<std::shared_ptr<File>>(a);
    // if (a->Path() < b->Path())
    //  return true;
    // else if (a->S)
    return a->Path() < b->Path();
}

size_t Directory::EntityCount () {
    return m_Contents.size();
}

Directory Directory::operator + (const Directory & dir) const {
    Directory merged(m_Path);

    std::merge(m_Contents.begin(), m_Contents.end(),
                    dir.m_Contents.begin(), dir.m_Contents.end(),
                    std::inserter(merged.m_Contents, merged.m_Contents.begin()),
                    Directory::compare);

    return merged;
}
