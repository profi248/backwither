#include <algorithm>
#include <fstream>
#include <filesystem>
#include <utility>
#include "BackupJob.h"
#include "FilesystemUtils.h"
#include "FileChunker.h"
#include "UserInterface.h"
#include "SQLiteBackupIndexProvider.h"

namespace fs = std::filesystem;

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool compressed, int64_t id) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Compressed (compressed),
        m_ID (id) {}

int BackupJob::Backup (UserInterface* ui) {
    // fixme with make_unique destructor gets called and I don't know why
    std::unique_ptr<BackupIndexProvider> config = std::unique_ptr<SQLiteBackupIndexProvider>
                                                  (new SQLiteBackupIndexProvider(this));
    fs::path source = fs::path(GetSource());
    fs::path destination = fs::path(GetDestination());

    FilesystemUtils::VerifySourceDirectory(source);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(destination);

    Directory prevState = config->LoadSnapshotFileIndex(-1);
    Directory currentState = FilesystemUtils::BrowseFolderRecursive(source);
    // Directory diff = currentState - prevState;
    // todo improve
    // add file comparator

    int64_t newSnapshotId = config->SaveSnapshotFileIndex(currentState);

    DirectoryIterator it(& currentState);
    size_t cnt = 0;

    while (!it.End()) {
        if (ui)
            ui->UpdateProgress(cnt, 0, it.GetPath(), it.GetSize());
        FileChunker::SaveFileChunks(source / it.GetPath(), it.GetID(), destination,
                                    newSnapshotId, config, IsCompressed());
        it++;
        cnt++;
    }

    return 0;
}

int BackupJob::Restore (UserInterface* ui, int64_t snapshotId) {
    std::unique_ptr<BackupIndexProvider> config =  std::unique_ptr<SQLiteBackupIndexProvider>
                                                   (new SQLiteBackupIndexProvider(this));
    fs::path restoreFrom = fs::path(GetDestination());
    fs::path restoreTo = fs::path(GetSource());

    // todo change the verification here, this is trash - maybe refactor
    FilesystemUtils::VerifySourceDirectory(restoreFrom);
    FilesystemUtils::VerifyOrCreateDestinationDirectory(restoreTo);

    Directory snapshotFiles = config->LoadSnapshotFileIndex(-1); // todo fix this snapshotID trash

    DirectoryIterator it(& snapshotFiles);
    size_t cnt = 0;

    while (!it.End()) {
        if (ui)
            ui->UpdateProgress(cnt, 0, it.GetPath(), it.GetSize());
        ChunkList fileChunks = config->RetrieveFileChunks(this, snapshotId, it.GetID());
        FilesystemUtils::RestoreFileFromChunks(restoreFrom, restoreTo, fileChunks,
                                               restoreTo / it.GetPath(), IsCompressed());
        it++;
        cnt++;
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

bool BackupJob::IsCompressed () const {
    return m_Compressed;
}

int64_t BackupJob::GetID () const {
    return m_ID;
}
