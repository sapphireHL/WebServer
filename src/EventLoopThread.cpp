#include "EventLoopThread.h"
#include "EventLoop.h"
#include "MutexLockGuard.h"
#include <unistd.h>
#include <sys/syscall.h>

using namespace std;

EventLoopThread::EventLoopThread():
    exiting(false),
    loop(nullptr),
    thread(bind(&EventLoopThread::threadFunc, this), "EventLoopThread"),
    mutex(),
    cond(mutex)
{
    //ctor
}

EventLoopThread::~EventLoopThread()
{
    //dtor
    exiting = true;
    if(loop){
        loop->quit();
        thread.join();
    }
}

EventLoop* EventLoopThread::startLoop()
{
    thread.start();
    //wait for loop has a value
    {
        MutexLockGuard lock(mutex);
        while(!loop){
            cond.wait();
        }
    }
    return loop;
}

//new thread logic
void EventLoopThread::threadFunc()
{

    EventLoop lp;

    {
        MutexLockGuard lock(mutex);
        loop = &lp;
        cond.notify();
    }
    loop->loop();
    loop = nullptr;
}
