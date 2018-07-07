#ifndef MUTEXLOCKGUARD_H
#define MUTEXLOCKGUARD_H

#include "MutexLock.h"
#include <assert.h>

class MutexLockGuard
{
    public:
        explicit MutexLockGuard(MutexLock& mutex):mutex_(mutex)
        {
            mutex_.lock();
        }

        ~MutexLockGuard()
        {
            mutex_.unlock();
        }

    private:
        MutexLockGuard(const MutexLockGuard&);
        MutexLockGuard& operator=(const MutexLockGuard&);

    private:
        MutexLock mutex_;
};

#define MutexLockGuard(x) static_assert(false, "missing mutex guard var name")

#endif // MUTEXLOCKGUARD_H
