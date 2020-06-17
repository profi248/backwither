# based on makefile from proseminar
CXX=g++
LD=g++
CXXFLAGS=-std=c++17 -Wall -pedantic -Wextra -O3
LDFLAGS=-lsqlite3 -lcrypto -lz -lstdc++fs
FILES= \
	src/bin/intermediate/File.o src/bin/intermediate/Directory.o \
	src/bin/intermediate/BackupJob.o src/bin/intermediate/BackupPlan.o src/bin/intermediate/TimedBackupJob.o \
	src/bin/intermediate/SQLiteConfigProvider.o src/bin/intermediate/FilesystemUtils.o \
	src/bin/intermediate/FilesystemChunkStorageProvider.o src/bin/intermediate/CompressedFilesystemChunkStorageProvider.o \
	src/bin/intermediate/UserInterface.o src/bin/intermediate/TerminalUserInterface.o \
	src/bin/intermediate/TimeDirectoryComparator.o src/bin/intermediate/DirectoryDiffIterator.o \
	src/bin/intermediate/BackupPlanIterator.o src/bin/intermediate/DirectoryIterator.o src/bin/intermediate/FileChunker.o \
	src/bin/intermediate/Chunk.o src/bin/intermediate/ChunkList.o src/bin/intermediate/ChunkListIterator.o \
	src/bin/intermediate/SQLiteBackupIndexProvider.o src/bin/intermediate/Snapshot.o \
	src/bin/intermediate/SnapshotList.o src/bin/intermediate/SnapshotListIterator.o src/bin/intermediate/TimeUtils.o

.PHONY: all clean run doc prepare

all: compile doc

test: CXXFLAGS += -fsanitize=address -g
test: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first
debug: CXXFLAGS += -fsanitize=address -g
debug: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first

debug-noasan:  CXXFLAGS += -g

doc:
	doxygen

debug: prepare backwither
debug-noasan: prepare backwither

compile: prepare backwither
	cp src/bin/backwither kostada2

prepare:
	mkdir -p src/bin/intermediate > /dev/null  2>&1

run: prepare backwither
	src/bin/backwither

test: $(FILES) | prepare
	$(CXX) $(CXXFLAGS) -c -o src/bin/intermediate/testbench.o src/tests/testbench.cpp
	$(LD) -o src/bin/test $^ src/bin/intermediate/testbench.o $(LDFLAGS)
	src/bin/test

backwither: $(FILES) src/bin/intermediate/main.o
	$(LD) -o src/bin/$@ $^ $(LDFLAGS)

clean:
	rm -f  src/bin/backwither
	rm -rf src/bin/intermediate/*
	rm -f  kostada2
	rm -rf doc

src/bin/intermediate/%o: src/%cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
