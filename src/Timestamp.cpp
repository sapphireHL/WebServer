#include "Timestamp.h"
#include <stdio.h>

using namespace std;

string Timestamp::toString() const
{
    int64_t seconds = microSecondsSinceEpoch / kMicroSecondsPerSecond;
    int64_t microSeconds = microSecondsSinceEpoch % kMicroSecondsPerSecond;
    char buf[32] = {0};
    sprintf(buf, "%ld.%06ld", seconds, microSeconds);
    return buf;
}

string Timestamp::toFormatString(bool showMicroSecond) const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / kMicroSecondsPerSecond);
    struct tm tm_time;
    gmtime_r(&seconds, &tm_time);

    if(showMicroSecond)
    {
        int microSeconds = static_cast<int>(microSecondsSinceEpoch % kMicroSecondsPerSecond);
        sprintf(buf, "%04d%02d%02d %02d:%02d:%02d.%06d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
                microSeconds);
    }
    else
    {
        sprintf(buf, "%04d%02d%02d %02d:%02d:%02d",
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    }
    return buf;
}

Timestamp Timestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    int64_t seconds = tv.tv_sec;
    return Timestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

Timestamp Timestamp::invalid()
{
    return Timestamp();
}
