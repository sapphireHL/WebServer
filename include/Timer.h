#ifndef TIMER_H
#define TIMER_H

#include "Timestamp.h"
#include <functional>
#include <atomic>

typedef std::function<void()> TimerCallBack;

class Timer
{
    public:
        //interv为0表示不需要重复执行
        Timer(const TimerCallBack &cb, Timestamp when, double interv):
            callback_(cb),
            expiration_(when),
            interval_(interv),
            repeat_(interv > 0),
            sequence_(numCreated++)
            {}
        ~Timer(){};

        void run()
        {
            callback_();
        }

        Timestamp expiration() const{ return expiration_; }
        bool repeat() const { return repeat_; }
        int sequence() const { return sequence_; }

        void restart(Timestamp now);//重置任务

    private:
        TimerCallBack callback_;//任务处理函数
        Timestamp expiration_;//过期时间，不断更新
        double interval_;//定时任务的间隔
        bool repeat_;//是否重复执行
        const int sequence_;//本任务序列号

        static std::atomic_int numCreated;//定时任务数量
};

#endif // TIMER_H
