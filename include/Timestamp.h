#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <algorithm>
#include <string>
#include <sys/time.h>

class Timestamp
{
    public:
        Timestamp() : microSecondsSinceEpoch(0)
        {
        }
        explicit Timestamp(int64_t microseconds) : microSecondsSinceEpoch(microseconds)
        {
        }

        ~Timestamp(){};

        void swap(Timestamp& that)
        {
            std::swap(microSecondsSinceEpoch, that.microSecondsSinceEpoch);
        }

        std::string toString() const;
        std::string toFormatString(bool showMicroSecond= true) const;
        bool valid()const { return microSecondsSinceEpoch > 0; }

        int64_t microSeconds() const { return microSecondsSinceEpoch; }
        time_t seconds() const
        {
            return static_cast<time_t>(microSecondsSinceEpoch / kMicroSecondsPerSecond);
        }

        static Timestamp now();
        static Timestamp invalid();//获取一个不可用的时间戳
        static const int kMicroSecondsPerSecond = 1000 * 1000;


    private:
        int64_t microSecondsSinceEpoch;//微秒表示的时间
};

inline bool operator < (const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSeconds() < rhs.microSeconds();
}

inline bool operator == (const Timestamp& lhs, const Timestamp& rhs)
{
    return lhs.microSeconds() == rhs.microSeconds();
}

inline double timeDifference(const Timestamp& high, const Timestamp& low)
{
    int64_t diff = high.microSeconds() - low.microSeconds();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;//整数部分以秒为单位
}

//加上一个秒数返回时间戳
inline Timestamp addTime(Timestamp t, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(t.microSeconds() + delta);
}

#endif // TIMESTAMP_H
