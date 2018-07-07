#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include <pthread.h>
#include <vector>
#include <memory>
#include <unordered_map>
#include "MutexLock.h"
#include "Timestamp.h"
#include "TimerId.h"
#include "Timer.h"
#include "TypeDefines.h"

class Poller;
class Channel;
class TimerQueue;

class EventLoop
{
    public:
        typedef std::function<void()> Functor;
        EventLoop();
        ~EventLoop();

        void loop();
        void quit();
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);

        TimerId runAt(const TimerCallBack& cb, const Timestamp& time);
        TimerId runAfter(const TimerCallBack& cb, double delay);
        TimerId runEvery(const TimerCallBack& cb, double interval);

        static EventLoop* getEventLoopOfCurrentThread();

        void queueInLoop(const Functor& f, int fd);

    private:
        typedef std::vector<Channel*> ChannelList;

        void processFunctors();
        void handleRead();
        void wakeup();

        bool looping_;
        bool quit_;
        const pid_t threadID_;
        std::unique_ptr<Poller> poller_;
        std::unique_ptr<TimerQueue> timerQueue_;
        ChannelList activeChannels_;
        MutexLock mutex_;
        std::unordered_map<int, Functor> functors;
        int wakeupFd;
        std::unique_ptr<Channel> wakeupChannel;
};

#endif // EVENTLOOP_H
