#ifndef BACKWITHER_TIMEUTILS_H
#define BACKWITHER_TIMEUTILS_H
#include <string>
#include <ctime>
#include <chrono>

/**
 * Provides various tools to compute and format time-related things.
 */

class TimeUtils {
    static constexpr char FORMAT[] = "%F %T";
public:
    /**
     * Weekdays compatible with tm::tm_wday.
     */
    enum weekday_t { NONE = -1, /**< Invalid day */
                     SUN = 0,   /**< Sunday */
                     MON = 1,   /**< Monday */
                     TUE = 2,   /**< Tuesday */
                     WED = 3,   /**< Wednesday */
                     THU = 4,   /**< Thursday */
                     FRI = 5,   /**< Friday */
                     SAT = 6    /**< Saturday */
    };

    /**
     * Indicator of unspecified time/plan.
     */
    static constexpr char ZERO[]   = "---";

    /**
     * Converts Unix timestamp to human readable string (dictated by internal const FORMAT).
     * @param timestamp Unix timestamp.
     * @return Datetime string.
     */
    static std::string   HumanDateTime (long long timestamp);

    /**
     * Converts backup plan to human readabale string.
     * @param day Planned weekday.
     * @param secondsSinceStart Planned at seconds since start of day.
     * @return Formatted string.
     */
    static std::string   PlanToString  (weekday_t day, int secondsSinceStart);

    /**
     * Last time when backup plan was supposed to trigger.
     * @param day Planned weekday.
     * @param secondsSinceStart Planned at seconds since start of day.
     * @return Unix timestamp of last planned backup trigger.
     */
    static long long     PlanLastScheduledTime (weekday_t day, int secondsSinceStart);

    /**
     * Retrieve current UTC Unix timestamp (or convert exisiting one to UTC).
     * @param timestamp Optional timestamp to convert.
     * @return Unix timestamp in UTC.
     */
    static long long int GetUTCTimestamp (time_t timestamp = -1);

    /**
     * Parse string to weekday_t.
     * @param wday String of mo-su (case agnostic).
     * @return Corresponding weekday_t (NONE value on failure).
     */
    static TimeUtils::weekday_t StringToWeekday (std::string wday);

    /**
     * Parse HH:MM string in local time to seconds since day started in UTC.
     * @param localtime Time string in HH:MM format (24h) in local time.
     * @return Seconds since day stared in UTC.
     */
    static int StringToUTCSecondsSinceStart (std::string localtime);

    /**
     * Parse XX:YY string into XX and YY.
     * @param nums The sting with numbers separated by colon.
     * @return Pair {XX, YY}.
     */
    static std::pair<int64_t, int64_t> ParsePosColumnSeparatedInts (std::string nums);

    /**
     * Compute number of seconds per day.
     * @return Number of seconds per day.
     */
    static constexpr int SecondsPerDay ();

    // from https://stackoverflow.com/a/58237530/2465760
    /**
     * Convert any td::chrono::time_point to time_t.
     * There's no real better way until C++20.
     * From https://stackoverflow.com/a/58237530.
     *
     * @tparam TP std::chrono::time_point type.
     * @param tp Time to convert
     * @return Converted time in time_t.
     */
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
