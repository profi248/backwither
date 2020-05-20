#include <getopt.h>
#include "main.h"
#include "FilesystemBrowser.h"
#include "SQLiteConfigProvider.h"

int main (int argc, char** argv) {
    // use getopt for parsing arguments
    // getopt(argc, argv, "");
    Directory* root = FilesystemBrowser::BrowseFolderRecursive("/var/home/david/ProgTest/PA2/kostada2/examples");
    auto cf = new SQLiteConfigProvider();
    cf->LoadBackupPlan();
    return 0;
}
