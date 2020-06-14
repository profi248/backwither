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
                        FilesystemEntity::Cmp);

    return diff;
}

Directory::~Directory () {
    m_Contents.clear();
}

size_t Directory::EntityCount () {
    return m_Contents.size();
}

Directory Directory::operator + (const Directory & dir) const {
    Directory merged(m_Path);

    std::merge(m_Contents.begin(), m_Contents.end(),
               dir.m_Contents.begin(), dir.m_Contents.end(),
               std::inserter(merged.m_Contents, merged.m_Contents.begin()),
               FilesystemEntity::Cmp);

    return merged;
}
