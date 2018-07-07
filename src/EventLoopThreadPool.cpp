#include "EventLoopThreadPool.h"
#include "EventLoopThread.h"

using namespace std;

EventLoopThreadPool::EventLoopThreadPool(EventLoop* base):
    baseLoop(base),
    started(false),
    nums(1),
    next(0)
{
    //ctor
}

EventLoopThreadPool::~EventLoopThreadPool()
{
    //dtor
}

void EventLoopThreadPool::start()
{
    started = true;
    for(int i=0; i<nums; ++i){
        EventLoopThread* th = new EventLoopThread();
        threads.push_back(shared_ptr<EventLoopThread>(th));
        //start thread and loop
        loops.push_back(th->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
    return loops[1];
    EventLoop* loop = baseLoop;
    if(loops.size()){
        loop = loops[next++];
        if(next >= (int)loops.size())
            next = 0;
    }
    return loop;
}

vector<EventLoop*> EventLoopThreadPool::getAllLoops()
{
    if(loops.empty()){
        loops.push_back(baseLoop);
    }
    return loops;
}
