#include <algorithm>
#include <fstream>
#include <filesystem>
#include <utility>
#include "BackupJob.h"
#include "FilesystemBrowser.h"
#include "FileChunker.h"

BackupJob::BackupJob (std::string source, std::string destination, std::string name, bool incremental, int64_t id) :
        m_SourcePath (std::move(source)),
        m_DestinationPath (std::move(destination)),
        m_Name (std::move(name)),
        m_Incremental (incremental),
        m_ID (id) {}

int BackupJob::DoBackup (ConfigProvider* config) {
    std::string source = GetSource();
    std::string destination = GetDestination();

    FilesystemBrowser::VerifySourceDirectory(source);
    FilesystemBrowser::VerifyOrCreateDestinationDirectory(destination);

    if (!GetIncremental())
        throw std::logic_error("Not implemented"); // fixme implement !incremental

    Directory prevState = config->LoadSnapshotFileIndex(this);
    Directory currentState = FilesystemBrowser::BrowseFolderRecursive(source);
    Directory diff = currentState - prevState;

    config->SaveSnapshotFileIndex(diff, this);

    DirectoryIterator it(& currentState);

    while (!it.End()) {
        FileChunker::GenerateFileChunks(it.GetPath());
        it++;
    }


    // todo split files into chunks, deduplicate chunks, save chunks index into db and chunk data on fs

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