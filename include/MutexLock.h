#ifndef MUTEXLOCK_H
#define MUTEXLOCK_H

#include "pthread.h"
#include "stdio.h"
class MutexLockGuard;
class Condition;

class MutexLock
{
    friend class MutexLockGuard;
    friend class Condition;

    public:
        MutexLock(){
            pthread_mutex_init(&mutex, nullptr);
        }

        ~MutexLock(){
            //printf("mutex 88\n");
            pthread_mutex_destroy(&mutex);
        }
    private:
        //MutexLock(const MutexLock&);
        MutexLock& operator=(const MutexLock&);

    private:
        //
        void lock(){
            pthread_mutex_lock(&mutex);
        }

        void unlock(){
            pthread_mutex_unlock(&mutex);
        }
        //
        pthread_mutex_t* getPthreadMutex(){
            return &mutex;
        }


    private:
        pthread_mutex_t mutex;
};

#endif // MUTEXLOCK_H
