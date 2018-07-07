#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <functional>
#include <memory>
#include <string>
#include <atomic>

class Thread
{
    public:
        typedef std::function<void ()> ThreadFunc;//线程回调函数

        explicit Thread(const ThreadFunc&, const std::string &name = std::string());
        ~Thread();

        void start();
        int join();

        bool started() const { return started_; }
        pid_t tid() const { return *tid_; }
        const std::string& name() const { return name_; }




    private:
        void setDefaultName();

        bool started_;
        bool joined_;
        pthread_t pthreadId_;
        std::shared_ptr<pid_t> tid_;
        ThreadFunc func_;
        std::string name_;

        static std::atomic_int numCreated;
};

#endif // THREAD_H
