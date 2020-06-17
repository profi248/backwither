# based on makefile from proseminar
CXX=g++
LD=g++
BUILDDIR=src/bin
OBJDIR=$(BUILDDIR)/intermediate
CXXFLAGS=-std=c++17 -Wall -pedantic -Wextra -O3
LDFLAGS=-lsqlite3 -lcrypto -lz -lstdc++fs
FILES= \
	$(OBJDIR)/Filesystem/File.o $(OBJDIR)/Filesystem/Directory.o \
	$(OBJDIR)/Backup/BackupJob.o $(OBJDIR)/Backup/BackupPlan.o $(OBJDIR)/Backup/TimedBackupJob.o \
	$(OBJDIR)/Backup/SQLiteConfigProvider.o $(OBJDIR)/Filesystem/FilesystemUtils.o \
	$(OBJDIR)/Filesystem/FilesystemChunkStorageProvider.o $(OBJDIR)/Filesystem/CompressedFilesystemChunkStorageProvider.o \
	$(OBJDIR)/UI/UserInterface.o $(OBJDIR)/UI/TerminalUserInterface.o \
	$(OBJDIR)/Filesystem/TimeDirectoryComparator.o $(OBJDIR)/Iterators/DirectoryDiffIterator.o \
	$(OBJDIR)/Iterators/BackupPlanIterator.o $(OBJDIR)/Iterators/DirectoryIterator.o $(OBJDIR)/Filesystem/FileChunker.o \
	$(OBJDIR)/Backup/Chunk.o $(OBJDIR)/Backup/ChunkList.o $(OBJDIR)/Iterators/ChunkListIterator.o \
	$(OBJDIR)/Backup/SQLiteBackupIndexProvider.o $(OBJDIR)/Backup/Snapshot.o \
	$(OBJDIR)/Backup/SnapshotList.o $(OBJDIR)/Iterators/SnapshotListIterator.o $(OBJDIR)/Backup/TimeUtils.o

.PHONY: all clean run doc prepare test debug debug-noasan backwither-testbench

all: compile doc

test: CXXFLAGS += -fsanitize=address -g
test: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first
debug: CXXFLAGS += -fsanitize=address -g
debug: LDFLAGS := -lasan $(LDFLAGS) # AddressSanitizer needs to be linked first

debug-noasan: CXXFLAGS += -g

doc:
	doxygen

debug: prepare backwither
debug-noasan: prepare backwither


compile: prepare backwither
	cp src/bin/backwither kostada2

# force non-parellel
prepare:
	mkdir -p $(BUILDDIR) > /dev/null 2>&1; \
	mkdir -p $(OBJDIR) > /dev/null 2>&1; \
	mkdir -p $(OBJDIR)/Iterators > /dev/null 2>&1; \
	mkdir -p $(OBJDIR)/UI > /dev/null 2>&1; \
	mkdir -p $(OBJDIR)/Backup > /dev/null 2>&1; \
	mkdir -p $(OBJDIR)/Filesystem > /dev/null 2>&1

test: prepare backwither-testbench
	src/bin/test

backwither-testbench: $(FILES)
	$(CXX) $(CXXFLAGS) -c -o $(OBJDIR)/testbench.o src/tests/testbench.cpp
	$(LD) -o src/bin/test $^ $(OBJDIR)/testbench.o $(LDFLAGS)

backwither: $(FILES) $(OBJDIR)/main.o
	$(LD) -o src/bin/$@ $^ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR)/*
	rm -f  $(BUILDDIR)/backwither
	rm -f  kostada2
	rm -rf doc

$(OBJDIR)/%o: src/%cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<
