#include <iostream>
#include <exception>

#include "FilesystemUtils.h"
#include "SQLiteConfigProvider.h"
#include "TerminalUserInterface.h"

int main (int argc, char** argv) {
    TerminalUserInterface tui;

    try {
        return tui.StartInterface(argc, argv);
    } catch (std::exception const & e) {
        std::cerr << "Unexpected fatal error: " << e.what() << std::endl;
    }

    // Directory* root = FilesystemUtils::BrowseFolderRecursive("/var/home/david/ProgTest/PA2/kostada2/examples");
}
