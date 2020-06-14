#include "TimeDirectoryComparator.h"

Directory TimeDirectoryComparator::CompareDirs (Directory prev, Directory curr) {
    Directory changedFiles("/");
    Directory newFiles = curr - prev;
    Directory deletedFiles = prev - curr;

    // files that appear in both directory states, with old timestamps
    Directory potentiallyChangedFilesPrevious = prev - deletedFiles - newFiles;
    // identical list of files, but with new timestamp to compare
    Directory potentiallyChangedFilesCurrent = curr - deletedFiles - newFiles;

    DirectoryIterator prevIt(& potentiallyChangedFilesPrevious);
    DirectoryIterator currIt(& potentiallyChangedFilesCurrent);

    // std::set is guaranteed to be sorted by path
    while (!prevIt.End() && !currIt.End()) {
        if (currIt.GetMtime() > prevIt.GetMtime())
            changedFiles.AddFilesystemEntity(currIt.Current());
        currIt++;
        prevIt++;
    }

    return changedFiles + newFiles;
}