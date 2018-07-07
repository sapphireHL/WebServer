#include "Thread.h"
#include <assert.h>

using namespace std;

struct ThreadData
{
    typedef Thread::ThreadFunc ThreadFunc;

    ThreadFunc func_;
    string name_;
    weak_ptr<pid_t> wtid_;

    ThreadData(const ThreadFunc& func,
               const string & name,
               const shared_ptr<pid_t>& tid):
        func_(func),
        name_(name),
        wtid_(tid)
    {
    }

    void runInThread()
    {
        try
        {
            func_();
        }
        catch(...)
        {

        }
    }
};

void *startThread(void* obj)
{
    ThreadData* data = static_cast<ThreadData*>(obj);
    data->runInThread();
    delete data;
    return nullptr;
}



atomic_int Thread::numCreated;

Thread::Thread(const ThreadFunc& func, const string &name):
    started_(false),
    joined_(false),
    pthreadId_(0),
    tid_(new pid_t(0)),
    func_(func),
    name_(name)
{
    //ctor
    setDefaultName();
}

Thread::~Thread()
{
    //dtor
    if(started_ && !joined_){
        pthread_detach(pthreadId_);
    }
}

void Thread::setDefaultName()
{
    ++numCreated;
    int num = numCreated;
    if(name_.empty()){
        name_ = "Thread" + to_string(num);
    }
}

void Thread::start()
{
    started_ = true;
    ThreadData* data = new ThreadData(func_, name_, tid_);
    if(pthread_create(&pthreadId_, nullptr, &startThread, data))
    {
        started_ = false;
        delete data;
    }
}

int Thread::join()
{
    assert(!joined_);
    joined_ = true;
    return pthread_join(pthreadId_, nullptr);
}
