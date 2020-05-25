#include "File.h"
File::File(std::string path, size_t mtime, size_t size) :
    m_Size (size),
    m_Mtime(mtime) { m_Path = path; }

size_t File::Size () {
    return m_Size;
}

size_t File::Mtime () {
    return m_Mtime;
}

