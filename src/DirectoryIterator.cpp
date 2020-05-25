#include <memory>

#include "BackupPlanIterator.h"
#include "DirectoryIterator.h"
#include "File.h"

DirectoryIterator::DirectoryIterator (Directory * dir) :
        m_Directory(dir),
        m_DirectoryIt (dir->m_Contents.begin()) {}

void DirectoryIterator::Next () {
    m_DirectoryIt++;
}

bool DirectoryIterator::End () const {
    return m_DirectoryIt == m_Directory->m_Contents.end();
}

bool DirectoryIterator::Empty () const {
    return m_Directory->m_Contents.empty();
}

std::string DirectoryIterator::GetPath () const {
    return (*m_DirectoryIt)->Path();
}

int64_t DirectoryIterator::GetMtime () const {
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirectoryIt))
        return file->Mtime();
    else
        return 0;
}

int64_t DirectoryIterator::GetSize () const {
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirectoryIt))
        return file->Size();
    else
        return 0;
}

DirectoryIterator DirectoryIterator::operator ++ (int) {
    Next();
    return *this;
}

