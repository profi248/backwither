#include <ctime>
#include "TimeUtils.h"

std::string TimeUtils::HumanDateTime (long long timestamp) {
    if (timestamp <= 0)
        return ZERO;
    time_t time = static_cast<time_t>(timestamp);
    char buf[100];
    std::strftime(buf, sizeof(buf), FORMAT, std::localtime(& time));
    return std::string(buf);
}
