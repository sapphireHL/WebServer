#ifndef EVENTLOOPTHREAD_H
#define EVENTLOOPTHREAD_H

#include "Condition.h"
#include "MutexLock.h"
#include "Thread.h"

class EventLoop;

class EventLoopThread
{
    public:
        EventLoopThread();
        ~EventLoopThread();

        EventLoop* startLoop();

    private:
        void threadFunc();
        bool exiting;
        EventLoop* loop;
        Thread thread;
        MutexLock mutex;
        Condition cond;
};

#endif // EVENTLOOPTHREAD_H
