#ifndef TIMERID_H_INCLUDED
#define TIMERID_H_INCLUDED

class Timer;
class TimerId
{
public:
    TimerId(): timer(nullptr),sequence(0) {};
    TimerId(Timer* t, int64_t s): timer(t), sequence(s) {};

    friend class TimerQueue;

private:
    Timer* timer;
    int64_t sequence;
};

#endif // TIMERID_H_INCLUDED
