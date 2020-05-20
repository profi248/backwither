#include "main.h"
#include "FilesystemBrowser.h"
#include "SQLiteConfigProvider.h"
#include "TerminalUserInterface.h"

int main (int argc, char** argv) {
    TerminalUserInterface tui;

    return tui.StartInterface(argc, argv);

    Directory* root = FilesystemBrowser::BrowseFolderRecursive("/var/home/david/ProgTest/PA2/kostada2/examples");
    auto cf = new SQLiteConfigProvider();
    cf->LoadBackupPlan();
    return 0;
}
