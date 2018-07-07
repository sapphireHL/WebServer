#ifndef TIMERQUEUE_H
#define TIMERQUEUE_H

#include "Timestamp.h"
#include "Channel.h"
#include "Timer.h"
#include <vector>
#include <set>

class EventLoop;
class TimerId;

class TimerQueue
{
    public:
        TimerQueue(EventLoop* lp);
        ~TimerQueue();

        TimerId addTimer(const TimerCallBack& cb, Timestamp when, double interv);
        //void cancel(TimerId id);


    private:
        typedef std::pair<Timestamp, Timer*> Entry;//一个定时任务
        typedef std::set<Entry> TimerList;//任务集合，set自动排序

        void handleRead();
        std::vector<Entry> getExpired(Timestamp now);
        void reset(std::vector<Entry>& expired, Timestamp now);
        bool insert(Timer* timer);

        EventLoop* loop;
        const int timerfd;
        Channel timerfdChannel;
        TimerList timers;
};

#endif // TIMERQUEUE_H
