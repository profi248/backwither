#ifndef BACKWITHER_TIMEUTILS_H
#define BACKWITHER_TIMEUTILS_H
#include <string>

class TimeUtils {
    static constexpr char FORMAT[] = "%F %T";
    static constexpr char ZERO[]   = "---";
public:
    static std::string HumanDateTime (long long timestamp);
};


#endif //BACKWITHER_TIMEUTILS_H
