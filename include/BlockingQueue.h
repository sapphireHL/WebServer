#ifndef BLOCKINGQUEUE_H
#define BLOCKINGQUEUE_H

#include <deque>
#include "MutexLock.h"
#include "MutexLockGuard.h"
#include "Condition.h"

template<typename T>
class BlockingQueue
{
    public:
        BlockingQueue(): mutex_(),
                        notEmpty_(mutex_),
                        queue_()
        {
        }


        void put(const T& task){
            MutexLockGuard lock(mutex_);
            queue_.push_back(task);
            notEmpty_.notify();//唤醒等待任务的线程
        }

        T take(){
            MutexLockGuard lock(mutex_);
            //队列为空，等待
            while(queue_.empty()){
                notEmpty_.wait();
            }
            T task(queue_.front());
            queue_.pop_front();
            return task;
        }

        size_t size() const{
            MutexLockGuard lock(mutex_);
            return queue_.size();
        }
        //~BlockingQueue();


    private:
        mutable MutexLock mutex_;
        Condition notEmpty_;
        std::deque<T> queue_;
};

#endif // BLOCKINGQUEUE_H
