#include "TimerQueue.h"
#include "TimerId.h"
#include <string.h>
#include <sys/timerfd.h>
#include <unistd.h>
#include <assert.h>

using namespace std;

//创建timerfd
int createTimerfd()
{
    int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerfd < 0)
    {
        //error
        exit(0);
    }
    return timerfd;
}

// now时刻距离when还有多长时间，返回一个timespec结构体，主要用于下面的timerfd_settime函数
struct timespec howMuchTimeFromNow(Timestamp when)
{
    int64_t microseconds = when.microSeconds() - Timestamp::now().microSeconds();
    if(microseconds < 100)
        microseconds = 100;
    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / Timestamp::kMicroSecondsPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);
    return ts;
};

//根据过期时间重新设置timerfd
void resetTimerfd(int timerfd, Timestamp expiration)
{
    struct itimerspec newValue;
    bzero(&newValue, sizeof newValue);
    newValue.it_value = howMuchTimeFromNow(expiration);
    int ret = timerfd_settime(timerfd, 0, &newValue, nullptr);//设置定时器
    if(ret){
        //error
        exit(0);
    }
}

// 从timerfd中读取8字节无意义数据，如果不读取，会一直导致epoll触发
void readTimerfd(int timerfd, Timestamp now)
{
    uint64_t howmany;
    ssize_t n = read(timerfd, &howmany, sizeof howmany);
    if(n != sizeof howmany){
        //error
        exit(0);
    }
}

TimerQueue::TimerQueue(EventLoop* lp):
    loop(lp),
    timerfd(createTimerfd()),
    timerfdChannel(loop, timerfd),
    timers()
{
    //设置channel的回调函数
    timerfdChannel.setReadCallBack(bind(&TimerQueue::handleRead, this));
    //在epoll中开始监听timerfd
    timerfdChannel.enableReading();
}

TimerQueue::~TimerQueue()
{
    close(timerfd);
    for(auto it = timers.begin(); it != timers.end(); ++it){
        delete it->second;
    }
}

TimerId TimerQueue::addTimer(const TimerCallBack& cb, Timestamp when, double interv)
{
    Timer* timer = new Timer(cb, when, interv);
    bool earliestChanged = insert(timer);//插入并且返回是否是最早的任务
    //如果timer是最早的一个任务，那么我们需要根据timer的过期时间重置timerfd
    if(earliestChanged){
        resetTimerfd(timerfd, timer->expiration());
    }
    return TimerId(timer, timer->sequence());
}

// 这是Channel的回调函数，当timerfd可读触发epoll时，需要执行这个函数，执行所有的callback
void TimerQueue::handleRead()
{
    Timestamp now(Timestamp::now());
    readTimerfd(timerfd, now); // 读取8个字节，防止epoll再次触发
    // 取出所有的到期任务
    vector<Entry> expired = getExpired(now);
    for(auto it = expired.begin(); it != expired.end(); ++it)
    {
        it->second->run();//执行
    }
    // 重置定时器，主要是处理那些需要重复的任务
    reset(expired, now);
}

vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
    vector<Entry> expired;
    // 生成一个entry，用于比较，提供过期的界限
    Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));//最大的指针地址
    // 以sentry为上界，取出所有时间戳小于now的entry 此时begin和end之间，就是已经过期的任务
    // set中的任务是按照时间戳从小到大排列
    auto end = timers.lower_bound(sentry);
    assert(end == timers.end() || now < end->first);
    //拷贝
    copy(timers.begin(), end, back_inserter(expired));
    //删除过期的
    timers.erase(timers.begin(), end);
    return expired;
}

// 处理完任务后，需要根据是否重复，将某些任务重新放入
void TimerQueue::reset(vector<Entry>& expired, Timestamp now)
{
    Timestamp nextExpire; // 下次的任务到期时间
    for(auto it = expired.begin(); it != expired.end(); ++it){
        if(it->second->repeat()){
            it->second->restart(now);
            insert(it->second);
        }
        else
            delete it->second;
    }

    // 如果timers不为空，下次过期时间就是第一个任务的过期时间
    if(timers.size()){
        nextExpire = timers.begin()->second->expiration();
    }
    // 如果nextExpire合法，则重置timefd
    if(nextExpire.valid()){
        resetTimerfd(timerfd, nextExpire);
    }
}

bool TimerQueue::insert(Timer* timer)
{
    bool earliestChanged = false; // 所有任务的第一次过期时间是否被更改
    Timestamp when = timer->expiration();
    TimerList::iterator it = timers.begin();
    if (it == timers.end() || when < it->first)
    {
        // 如果timers为空，或者when小于目前最早的时间，那么最早过期时间，肯定被改变
        earliestChanged = true;
    }
    timers.insert(Entry(when, timer));
    return earliestChanged;
}
