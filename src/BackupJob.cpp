#include <utility>
#include "BackupJob.h"

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool incremental, int64_t id) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Incremental (incremental),
        m_ID (id) {}

std::string BackupJob::GetSource () const {
    return m_SourcePath;
}

std::string BackupJob::GetDestination () const {
    return m_DestinationPath;
}

std::string BackupJob::GetName () const {
    return m_Name;
}

bool BackupJob::GetIncremental () const {
    return m_Incremental;
}

int64_t BackupJob::GetID () const {
    return m_ID;
}
