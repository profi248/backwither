#include <getopt.h>
#include "main.h"
#include "FilesystemBrowser.h"

int main (int argc, char** argv) {
    // use getopt for parsing arguments
    FilesystemBrowser::BrowseFolderRecursive("testfiles");
    return 0;
}
