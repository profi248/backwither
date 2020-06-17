#include "File.h"
File::File (std::string path, long long mtime, size_t size, int64_t id) :
    m_Size (size),
    m_Mtime (mtime),
    m_ID (id) { m_Path = path; }

size_t File::GetSize () const {
    return m_Size;
}

long long File::GetMtime () const {
    return m_Mtime;
}

int64_t File::GetID () const {
    return m_ID;
}

void File::SetID (int64_t id) {
    m_ID = id;
}

