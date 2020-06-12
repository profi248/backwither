#include <ctime>
#include <sstream>
#include <iomanip>
#include "TimeUtils.h"

std::string TimeUtils::HumanDateTime (long long timestamp) {
    if (timestamp <= 0)
        return ZERO;
    time_t time = static_cast<time_t>(timestamp);
    char buf[100];
    std::strftime(buf, sizeof(buf), FORMAT, std::localtime(& time));
    return std::string(buf);
}

std::string TimeUtils::PlanToString (TimeUtils::weekday_t day, int secondsSinceStart) {
    std::ostringstream out;
    switch (day) {
        case MON:
            out << "Mon";
            break;
        case TUE:
            out << "Tue";
            break;
        case WED:
            out << "Wed";
            break;
        case THU:
            out << "Thu";
            break;
        case FRI:
            out << "Fri";
            break;
        case SAT:
            out << "Sat";
            break;
        case SUN:
            out << "Sun";
            break;
    }

    int hrs = 0, min = 0, sec = 0;
    while (secondsSinceStart >= 3600) {
        secondsSinceStart -= 3600;
        hrs++;
    }

    while (secondsSinceStart >= 60) {
        secondsSinceStart -= 60;
        min++;
    }

    sec = secondsSinceStart;

    out << " " << std::setfill('0')
               << std::setw(2) << hrs << ":"
               << std::setw(2) << min <<  ":"
               << std::setw(2) << sec;
    return out.str();
}
