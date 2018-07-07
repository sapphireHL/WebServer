#include "EventLoop.h"
#include "Channel.h"
#include "Poller.h"
#include "TimerQueue.h"
#include "MutexLockGuard.h"
#include <assert.h>
#include <poll.h>
#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace std;

int createEventFd()
{
    int evfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    assert(evfd >= 0);
    return evfd;
}

__thread EventLoop* t_LoopInThisThread = 0;
const int kPollTimeMs = 10000;

class IgnoreSigPipe
{
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
    }
};
IgnoreSigPipe init;

EventLoop::EventLoop():
    looping_(false),
    quit_(false),
    threadID_(pid_t(0)),
    poller_(new Poller(this)),
    timerQueue_(new TimerQueue(this)),
    wakeupFd(createEventFd()),
    wakeupChannel(new Channel(this, wakeupFd))
{
    //ctor
    wakeupChannel->setReadCallBack(bind(&EventLoop::handleRead, this));
    wakeupChannel->enableReading();
}

EventLoop::~EventLoop()
{
    //dtor
    wakeupChannel->disableAll();
    wakeupChannel->remove();
    close(wakeupFd);
    assert(!looping_);
    t_LoopInThisThread = nullptr;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread()
{
    return t_LoopInThisThread;
}

void EventLoop::loop()
{
    assert(!looping_);
    looping_ = true;
    quit_ = false;

    while(!quit_)
    {
        activeChannels_.clear();
        poller_->poll(kPollTimeMs, &activeChannels_);
        //printf("%d channels\n", activeChannels_.size());
        for(auto it = activeChannels_.begin(); it != activeChannels_.end(); ++it)
        {
            (*it)->handleEvent();
        }
        processFunctors();
    }
    looping_ = false;
}

void EventLoop::quit()
{
    quit_ = true;
}

void EventLoop::updateChannel(Channel* channel)
{
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
    channel->disableAll();
    poller_->removeChannel(channel);
}

TimerId EventLoop::runAt(const TimerCallBack& cb, const Timestamp& time)
{
    return timerQueue_->addTimer(cb, time, 0.0);
}

TimerId EventLoop::runAfter(const TimerCallBack& cb, double delay)
{
    Timestamp time(addTime(Timestamp::now(), delay));
    return runAt(cb, time);
}

TimerId EventLoop::runEvery(const TimerCallBack& cb, double interval)
{
    Timestamp time(addTime(Timestamp::now(), interval));
    return timerQueue_->addTimer(cb, time, interval);
}

void EventLoop::queueInLoop(const Functor& f, int fd)
{

    {
        //printf("%p\n", &mutex_);
        MutexLockGuard lock(mutex_);
        if(!functors.count(fd)){
            //printf("add in loop:%p,%p\n", this, &f);
            functors[fd] = f;
        }
    }
    wakeup();
}

void EventLoop::processFunctors()
{
    //use swap to reduce the area
    unordered_map<int, Functor> tmp;
    {
        MutexLockGuard lock(mutex_);
        tmp.swap(functors);
    }
    for(auto it:tmp){
        it.second();
    }
}

void EventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = read(wakeupFd, &one, sizeof one);
    assert(n == sizeof one);
}

void EventLoop::wakeup()
{
    uint64_t one = 1;
    //write to wake up
    ssize_t n = write(wakeupFd, &one, sizeof one);
    assert(n == sizeof one);
}
