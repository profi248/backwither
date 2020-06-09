#ifndef BACKWITHER_TIMEUTILS_H
#define BACKWITHER_TIMEUTILS_H
#include <string>
#include <ctime>
#include <chrono>


class TimeUtils {
    static constexpr char FORMAT[] = "%F %T";
    static constexpr char ZERO[]   = "---";
public:
    static std::string HumanDateTime (long long timestamp);

    // from https://stackoverflow.com/a/58237530/2465760
    // used to convert std::filesystem::file_time_type because there's no reasonable way
    template <typename TP>
    static std::time_t toTimeT (TP tp) {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                                                            + system_clock::now());
        return system_clock::to_time_t(sctp);
    }
};


#endif //BACKWITHER_TIMEUTILS_H
