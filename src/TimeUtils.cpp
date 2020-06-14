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

std::string TimeUtils::PlanToString (weekday_t day, int secondsSinceStart) {
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

    // adjust for DST and timezone
    time_t now = time(nullptr);
    tm* t = localtime(& now);
    if (t->tm_isdst)
        secondsSinceStart += 3600;
    secondsSinceStart -= timezone;

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

long long TimeUtils::PlanLastScheduledTime (weekday_t day, int secondsSinceStart) {
    // this time library is truly trash.
    std::time_t now = std::time(nullptr);
    std::tm nowTm   = *std::gmtime(& now);
    std::tm localTm = *std::localtime(& now);
    std::tm todayTm = nowTm;
    todayTm.tm_hour = 0;
    todayTm.tm_min  = 0;
    todayTm.tm_sec  = 0;

    std::time_t today = std::mktime(& todayTm);
    if (localTm.tm_isdst)
        today += 3600;

    int dayDiff = nowTm.tm_wday - day;
    dayDiff += 7;
    if (nowTm.tm_wday == day && today + secondsSinceStart < now) {
        // this day
        return today + secondsSinceStart;
    } else if (nowTm.tm_wday == day) {
        // last week
        return today + secondsSinceStart - 7 * SecondsPerDay();
    } else {
        return today + secondsSinceStart - dayDiff * SecondsPerDay();
    }
}

long long int TimeUtils::GetUTCTimestamp (time_t timestamp) {
    std::time_t now;
    if (timestamp > 0)
        now = timestamp;
    else
        now = std::time(nullptr);

    std::tm nowTm = *std::gmtime(& now);
    std::tm localTm = *std::localtime(& now);
    std::time_t gmt = std::mktime(& nowTm);
    if (localTm.tm_isdst)
        gmt += 3600;

    return gmt;
}

constexpr int TimeUtils::SecondsPerDay () {
    return 3600 * 24;
}
