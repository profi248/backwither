#include <iostream>

#include "FilesystemUtils.h"
#include "TerminalUserInterface.h"

int main (int argc, char** argv) {
    TerminalUserInterface* tui = new TerminalUserInterface();

    try {
        int ret = tui->StartInterface(argc, argv);
        delete tui;
        return ret;
    } catch (std::exception const & e) {
        delete tui;
        std::cerr << "Unexpected fatal error: " << e.what() << std::endl;
    }
}
