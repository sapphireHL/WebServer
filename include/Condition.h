#ifndef CONDITION_H
#define CONDITION_H

#include "MutexLock.h"
class Condition
{
    public:
        explicit Condition(MutexLock& mutex):mutex_(mutex)
        {
            pthread_cond_init(&pcond, nullptr);
        }
        ~Condition()
        {
            pthread_cond_destroy(&pcond);
        }

        void wait()
        {
            pthread_cond_wait(&pcond, mutex_.getPthreadMutex());
        }
        void notify()
        {
            pthread_cond_signal(&pcond);
        }
        void notifyAll()
        {
            pthread_cond_broadcast(&pcond);
        }


    private:
        MutexLock mutex_;
        pthread_cond_t pcond;
};

#endif // CONDITION_H
