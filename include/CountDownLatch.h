#ifndef COUNTDOWNLATCH_H
#define COUNTDOWNLATCH_H

#include "MutexLock.h"
#include "MutexLockGuard.h"
#include "Condition.h"

class CountDownLatch{
public:
	explicit CountDownLatch(int count);
	void wait();
	void countDown();
	int getCount() const;

private:
	mutable MutexLock mutex_;
	Condition condition_;
	int count_;
};

#endif // COUNTDOWNLATCH_H
