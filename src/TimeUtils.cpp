#include <ctime>
#include <sstream>
#include <cctype>
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
        case NONE:
            out << "Invalid";
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

TimeUtils::weekday_t TimeUtils::StringToWeekday (std::string wday) {
    std::string wdayLower;
    for (auto c : wday) {
        wdayLower.push_back(std::tolower(c));
    }

    if (wdayLower == "mo")
        return MON;
    else if (wdayLower == "tu")
        return TUE;
    else if (wdayLower == "we")
        return WED;
    else if (wdayLower == "th")
        return THU;
    else if (wdayLower == "fr")
        return FRI;
    else if (wdayLower == "sa")
        return SAT;
    else if (wdayLower == "su")
        return SUN;

    return NONE;
}

int TimeUtils::StringToUTCSecondsSinceStart (std::string str) {
    auto parsed = ParsePosColumnSeparatedInts(str);
    if (parsed.first > 23 || parsed.first < 0 || parsed.second > 59 || parsed.second < 0)
        return -1;
    int secondsSinceStart = parsed.first * 3600 + parsed.second * 60;

    // adjust for DST and timezone
    time_t now = time(nullptr);
    tm* t = localtime(& now);
    if (t->tm_isdst)
        secondsSinceStart -= 3600;
    secondsSinceStart += timezone;

    return secondsSinceStart;
}

constexpr int TimeUtils::SecondsPerDay () {
    return 3600 * 24;
}

std::pair<int64_t, int64_t> TimeUtils::ParsePosColumnSeparatedInts (std::string nums) {
    std::string rawA, rawB;
    int64_t a, b;
    bool second = false;

    for (char ch : nums) {
        if (ch == ':') {
            second = true;
            continue;
        }

        if (!second)
            rawA += ch;
        else
            rawB += ch;
    }

    if (!second) {
        throw std::invalid_argument("Number format is incorrect (numbers need to be separated by colon).");
    }

    try {
        a = stoll(rawA);
        b = stoll(rawB);
    } catch (std::exception &) {
        throw std::invalid_argument("Supplied number is not numeric.");
    }

    return std::pair {a, b};
}

