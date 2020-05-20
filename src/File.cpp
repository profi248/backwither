#include "File.h"
File::File (std::string path, size_t size) {
    m_Size = size;
    m_Path = path;
}
