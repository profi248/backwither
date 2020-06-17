#include <memory>
#include <limits>
#include <iostream>
#include "../Filesystem/File.h"
#include "DirectoryDiffIterator.h"
#include "../UI/UserInterface.h"

DirectoryDiffIterator::DirectoryDiffIterator (Directory * a, Directory * b) :
        m_DirA (a),
        m_DirB (b),
        m_DirAIt (a->m_Contents.begin()),
        m_DirBIt (b->m_Contents.begin()) { init(); }

void DirectoryDiffIterator::Next () {
    size_t mtimeA = 0, mtimeB = 0;

    do {
        m_DirAIt++;
        m_DirBIt++;

        if (!End()) {
            if (auto file = std::dynamic_pointer_cast<File>(*m_DirAIt))
                mtimeA = file->GetMtime();

            if (auto file = std::dynamic_pointer_cast<File>(*m_DirBIt))
                mtimeB = file->GetMtime();

            if (mtimeA != mtimeB)
                break;
        } else {
            break;
        }

    } while (!End());
}

bool DirectoryDiffIterator::End () const {
    return m_DirAIt == m_DirA->m_Contents.end() || m_DirBIt == m_DirB->m_Contents.end();
}

bool DirectoryDiffIterator::Empty () const {
    return End() || m_DirA->m_Contents.empty() || m_DirB->m_Contents.empty();
}

void DirectoryDiffIterator::Rewind () {
    m_DirAIt = m_DirA->m_Contents.begin();
    m_DirBIt = m_DirB->m_Contents.begin();
    init();
}

void DirectoryDiffIterator::operator ++ (int) {
    Next();
}

std::vector<std::string> DirectoryDiffIterator::TableHeader () const {
    return { "path", "size difference", "previously modified", "last modified" };
}

std::vector<std::string> DirectoryDiffIterator::TableRow () const {
    size_t fileSizeA = 0;
    if (End())
        return {};
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirAIt))
        fileSizeA = file->GetSize();

    size_t fileSizeB = 0;
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirBIt))
        fileSizeB = file->GetSize();

    size_t mtimeA = 0;
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirAIt))
        mtimeA = file->GetMtime();

    size_t mtimeB = 0;
    if (auto file = std::dynamic_pointer_cast<File>(*m_DirBIt))
        mtimeB = file->GetMtime();

    std::string sizeDiffFormatted;
    if (fileSizeA > std::numeric_limits<int64_t>::max() || fileSizeB > std::numeric_limits<int64_t>::max())
        sizeDiffFormatted = "too big!";
    else {
        int64_t sizeDiff = fileSizeB - fileSizeA;
        if (sizeDiff < 0) {
            sizeDiff = std::abs(sizeDiff);
            sizeDiffFormatted += "-";
        } else if (sizeDiff > 0) {
            sizeDiffFormatted += "+";
        }

        sizeDiffFormatted += UserInterface::HumanFileSize(sizeDiff);
    }

    if ((*m_DirAIt)->Path() != (*m_DirBIt)->Path())
        throw std::bad_exception();

    return { (*m_DirAIt)->Path(), sizeDiffFormatted, TimeUtils::HumanDateTime(mtimeA), TimeUtils::HumanDateTime(mtimeB) };
}

void DirectoryDiffIterator::init () {
    size_t mtimeA = 0, mtimeB = 0;

    if (auto file = std::dynamic_pointer_cast<File>(*m_DirAIt))
        mtimeA = file->GetMtime();

    if (auto file = std::dynamic_pointer_cast<File>(*m_DirBIt))
        mtimeB = file->GetMtime();

    if (mtimeA == mtimeB)
        Next();
}

