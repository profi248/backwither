# based on makefile from proseminar
CXX=g++
LD=g++
CXXFLAGS=-std=c++17 -Wall -pedantic
LDFLAGS=-lstdc++fs -lsqlite3 -lcrypto
FILES= \
	src/bin/intermediate/main.o src/bin/intermediate/FilesystemEntity.o src/bin/intermediate/File.o src/bin/intermediate/Directory.o \
	src/bin/intermediate/Link.o src/bin/intermediate/BackupJob.o src/bin/intermediate/BackupPlan.o src/bin/intermediate/TimedBackupJob.o \
	src/bin/intermediate/SQLiteConfigProvider.o src/bin/intermediate/FilesystemBrowser.o \
	src/bin/intermediate/BackupStorageProvider.o src/bin/intermediate/IncrementalFilesystemBackupStorageProvider.o \
	src/bin/intermediate/FilesystemBackupStorageProvider.o src/bin/intermediate/UserInterface.o src/bin/intermediate/TerminalUserInterface.o \
	src/bin/intermediate/FileComparator.o src/bin/intermediate/HashFileComparator.o src/bin/intermediate/TimeFileComparator.o \
	src/bin/intermediate/BackupPlanIterator.o src/bin/intermediate/DirectoryIterator.o src/bin/intermediate/FileChunker.o \
	src/bin/intermediate/Chunk.o src/bin/intermediate/ChunkList.o

.PHONY: all clean run doc

all: compile doc

debug: CXXFLAGS += -fsanitize=address -g
debug: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first
compile: CXXFLAGS += -O3

doc:
	doxygen

debug: backwither

compile: backwither
	cp src/bin/backwither kostada2

run: backwither
	src/bin/backwither

test: backwither
	$(CXX) $(CXXFLAGS) -o src/bin/test src/tests/test.cpp
	src/bin/test

backwither: $(FILES)
	$(LD) $(LDFLAGS) -o src/bin/$@ $^

clean:
	rm -f  src/bin/backwither
	rm -rf src/bin/intermediate/*
	rm -f  kostada2
	rm -rf doc

src/bin/intermediate/%o: src/%cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
