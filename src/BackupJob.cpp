#include <algorithm>
#include <fstream>
#include <filesystem>
#include <utility>
#include "BackupJob.h"
#include "FilesystemUtils.h"
#include "FileChunker.h"

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool incremental, int64_t id) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Incremental (incremental),
        m_ID (id) {}

int BackupJob::Backup (ConfigProvider* config) {
    std::filesystem::path source = GetSource();
    std::filesystem::path destination = GetDestination();

    FilesystemUtils::VerifySourceDirectory(source);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(destination);

    if (!GetIncremental())
        throw std::logic_error("Not implemented"); // fixme implement !incremental

    Directory prevState = config->LoadSnapshotFileIndex(this);
    Directory currentState = FilesystemUtils::BrowseFolderRecursive(source);
    // Directory diff = currentState - prevState;
    // todo improve

    int64_t newSnapshotId = config->SaveSnapshotFileIndex(currentState, this);

    DirectoryIterator it(& currentState);

    while (!it.End()) {
        FileChunker::SaveFileChunks(source / it.GetPath(), it.GetID(), destination, newSnapshotId, config);
        it++;
    }

    return 0;
}

int BackupJob::Restore (ConfigProvider* config, int64_t snapshotId) {
    std::filesystem::path restoreFrom = GetDestination();
    std::filesystem::path restoreTo = GetSource();

    // todo change the verification here, this is trash - maybe refactor
    FilesystemUtils::VerifySourceDirectory(restoreFrom);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(restoreTo);

    if (!GetIncremental())
        throw std::logic_error("Not implemented"); // fixme implement !incremental

    Directory snapshotFiles = config->LoadSnapshotFileIndex(this, -1); // todo fix this snapshotID trash

    DirectoryIterator it(& snapshotFiles);

    while (!it.End()) {
        ChunkList fileChunks = config->RetrieveFileChunks(this, snapshotId, it.GetID());
        FilesystemUtils::RestoreFileFromChunks(restoreFrom, restoreTo, fileChunks, restoreTo / it.GetPath());
        it++;
    }

    return 0;
}

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
