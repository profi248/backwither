#include "File.h"
File::File (std::string path, long long mtime, size_t size, int64_t id) :
    m_Size (size),
    m_Mtime (mtime),
    m_ID (id) { m_Path = path; }

size_t File::GetSize () {
    return m_Size;
}

size_t File::GetMtime () {
    return m_Mtime;
}

int64_t File::GetID () {
    return m_ID;
}

void File::SetID (int64_t id) {
    m_ID = id;
}

