#ifndef BACKWITHER_TIMEUTILS_H
#define BACKWITHER_TIMEUTILS_H
#include <string>
#include <ctime>
#include <chrono>
#include "BackupJob.h"


class TimeUtils {
    static constexpr char FORMAT[] = "%F %T";
public:
    // day order for compatibility with tm::tm_wday
    enum weekday_t {SUN = 0, MON, TUE, WED, THU, FRI, SAT};
    static constexpr char ZERO[]   = "---";

    static std::string   HumanDateTime (long long timestamp);
    static std::string   PlanToString  (weekday_t day, int secondsSinceStart);
    static long long     PlanLastScheduledTime (weekday_t day, int secondsSinceStart);
    static long long int GetUTCTimestamp (time_t timestamp = -1);
    static constexpr int SecondsPerDay ();

    // from https://stackoverflow.com/a/58237530/2465760
    // used to convert std::filesystem::file_time_type because there's no reasonable way
    template <typename TP>
    static std::time_t toTimeT (TP tp) {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                                                            + system_clock::now());
        std::time_t localStamp = system_clock::to_time_t(sctp);
        return GetUTCTimestamp(localStamp);
    }

};


#endif //BACKWITHER_TIMEUTILS_H
