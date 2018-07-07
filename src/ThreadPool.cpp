#include "ThreadPool.h"
#include <algorithm>

using namespace std;
using namespace std::placeholders;

ThreadPool::ThreadPool(const string& name):
    mutex_(),
    notEmpty_(mutex_),
    name_(name),
    queue_(),
    running_(false)
{
    //ctor
}

ThreadPool::~ThreadPool()
{
    //dtor
    if(running_)
    {
        stop();
    }
}

void ThreadPool::start(int num)
{
    assert(threads_.empty());
    running_ = true;
    threads_.reserve(num);
    for(int i = 0; i < num; ++i)
    {
        threads_.push_back(Thread(bind(&ThreadPool::runInThread, this),
                                  name_ + to_string(i)));
        threads_[i].start();
    }

    if(num == 0 && threadInitCallBack_)
    {
        threadInitCallBack_();
    }
}

void ThreadPool::stop()
{
    {
        MutexLockGuard lock(mutex_);
        running_ = false;
        notEmpty_.notifyAll();
    }

    for_each(threads_.begin(), threads_.end(), bind(&Thread::join, _1));
}

void ThreadPool::addTask(const Task& t)
{
    if(threads_.empty())
    {
        t();
    }
    else
    {
        queue_.put(t);
    }

}

ThreadPool::Task ThreadPool::take()
{
    Task task = queue_.take();
    return task;
}

void ThreadPool::runInThread()
{
    try
    {
        if(threadInitCallBack_)
        {
            threadInitCallBack_();
        }
        while(running_)
        {
            Task task(take());
            if(task)
            {
                task();
            }
        }
    }
    catch(...)
    {

    }
}
