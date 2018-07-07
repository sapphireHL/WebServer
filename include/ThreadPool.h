#ifndef THREADPOOL_H
#define THREADPOOL_H

#include "MutexLock.h"
#include "MutexLockGuard.h"
#include "Condition.h"
#include "BlockingQueue.h"
#include "Thread.h"
#include <functional>
#include <string>
#include <vector>

class ThreadPool
{
    public:
        typedef std::function<void()> Task;

        explicit ThreadPool(const std::string& name = std::string("ThreadPool"));
        ~ThreadPool();

        void setThreadInitCallBack(Task cb) { threadInitCallBack_ = cb; }

        void start(int numThreads);
        void stop();
        void addTask(const Task& t);


    private:
        void runInThread();
        Task take();

        MutexLock mutex_;
        Condition notEmpty_;
        std::string name_;
        Task threadInitCallBack_;
        std::vector<Thread> threads_;
        BlockingQueue<Task> queue_;
        bool running_;

};

#endif // THREADPOOL_H
