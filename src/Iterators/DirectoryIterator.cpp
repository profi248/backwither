#include <memory>
#include "../Filesystem/File.h"
#include "DirectoryIterator.h"
#include "../UI/UserInterface.h"

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

void DirectoryIterator::Rewind () {
    m_DirectoryIt = m_Directory->m_Contents.begin();
}

std::string DirectoryIterator::GetPath () const {
    return (*m_DirectoryIt)->Path();
}

int64_t DirectoryIterator::GetMtime () const {
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirectoryIt))
        return file->GetMtime();
    else
        return 0;
}

int64_t DirectoryIterator::GetSize () const {
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirectoryIt))
        return file->GetSize();
    else
        return 0;
}

int64_t DirectoryIterator::GetID () const {
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirectoryIt))
        return file->GetID();
    else
        return 0;
}

void DirectoryIterator::SetID (int64_t id) {
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirectoryIt))
        file->SetID(id);
}



void DirectoryIterator::operator ++ (int) {
    Next();
}

std::vector<std::string> DirectoryIterator::TableHeader () const {
    return { "path", "size", "last modified" };
}

std::vector<std::string> DirectoryIterator::TableRow () const {
    return { GetPath(), UserInterface::HumanFileSize(GetSize()), TimeUtils::HumanDateTime(GetMtime()) };
}

std::shared_ptr<FilesystemEntity> DirectoryIterator::Current () const {
    return *m_DirectoryIt;
}

