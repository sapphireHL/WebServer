#include "Timer.h"

std::atomic_int Timer::numCreated;


void Timer::restart(Timestamp now)
{
    if(repeat_)
    {
        //需要更新，计算下一次过期时间
        expiration_ = addTime(now, interval_);
    }
    else
    {
        //不需更新，赋为无效
        expiration_ = Timestamp::invalid();
    }
}
