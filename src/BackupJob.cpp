#include <utility>
#include "BackupJob.h"

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool incremental) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Incremental (incremental) {}
