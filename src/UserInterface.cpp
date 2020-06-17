#include <sstream>
#include <iomanip>
#include "UserInterface.h"


std::string UserInterface::HumanFileSize (size_t bytes) {
        std::ostringstream oss;

        // bypass float if division not needed
        if (bytes < 1024) {
            oss << bytes << " B";
            return oss.str();
        }

        double size = bytes;
        int divisions = 0;
        while (size >= 1024) {
            size /= 1024;
            divisions++;
        }

        int precision = (divisions < 3) ? 1 : 2;

        oss << std::fixed << std::setprecision(precision) << size;

        switch (divisions) {
            case 1:
                oss << " KiB";
                break;
            case 2:
                oss << " MiB";
                break;
            case 3:
                oss << " GiB";
                break;
            case 4:
                oss << " TiB";
                break;
            case 5:
                oss << " PiB";
                break;
            case 6:
                oss << " EiB";
                break;
            default:
                throw std::bad_exception();
        }

        return oss.str();
}

std::string UserInterface::GetVersion () {
    return "0.1.0";
}