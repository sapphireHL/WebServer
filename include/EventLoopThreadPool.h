#ifndef EVENTLOOPTHREADPOOL_H
#define EVENTLOOPTHREADPOOL_H

#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
    public:
        EventLoopThreadPool(EventLoop* base);
        ~EventLoopThreadPool();

        void setThreadNum(int num) { nums = num; }
        void start();

        EventLoop* getNextLoop();
        std::vector<EventLoop*> getAllLoops();
        bool isStarted() { return started; }

    private:
        EventLoop* baseLoop;//master loop, from main thread
        bool started;
        int nums;
        int next;//find next thread
        std::vector<std::shared_ptr<EventLoopThread>> threads;//threads vector
        std::vector<EventLoop*> loops;//loops respond to every thread
};

#endif // EVENTLOOPTHREADPOOL_H
