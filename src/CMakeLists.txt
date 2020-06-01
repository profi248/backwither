cmake_minimum_required(VERSION 3.16)
project(backwither)

set(CMAKE_CXX_STANDARD 17)

include_directories(..)


add_executable(backwither
        main.cpp FilesystemEntity.cpp File.cpp Directory.cpp Link.cpp
        FilesystemEntity.h File.h Directory.h Link.h BackupJob.cpp
        BackupJob.h BackupPlan.cpp BackupPlan.h TimedBackupJob.cpp TimedBackupJob.h
        ConfigProvider.h SQLiteConfigProvider.cpp SQLiteConfigProvider.h
        FilesystemUtils.cpp FilesystemUtils.h ChunkStorageProvider.cpp
        ChunkStorageProvider.h FilesystemChunkStorageProvider.cpp FilesystemChunkStorageProvider.h
        UserInterface.cpp UserInterface.h TerminalUserInterface.cpp TerminalUserInterface.h
        FileComparator.cpp FileComparator.h HashFileComparator.cpp HashFileComparator.h TimeFileComparator.cpp TimeFileComparator.h
        BackupPlanIterator.cpp BackupPlanIterator.h DirectoryIterator.cpp DirectoryIterator.h
        FileChunker.cpp FileChunker.h Chunk.cpp Chunk.h ChunkList.cpp ChunkList.h
        ChunkListIterator.cpp ChunkListIterator.h)

target_link_libraries(backwither -lsqlite3 -lcrypto)
