#include "Directory.h"

Directory::Directory (std::string path) {
    m_Path = path;
}

void Directory::AddFilesystemEntity (FilesystemEntity* e) {
    m_Contents.push_back(e);
}
