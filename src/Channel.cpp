#include "Channel.h"
#include <sys/epoll.h>
#include <assert.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop* loop, int fd):
    events_(0),
    revents_(0),
    loop_(loop),
    fd_(fd),
    index_(-1)
{
    //ctor
}

Channel::~Channel()
{
    //dtor
}

void Channel::update()
{
    loop_->updateChannel(this);
}

void Channel::handleEvent()
{
    if((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)){
        if(closeCallBack_) closeCallBack_();
    }
    if(revents_ & (EPOLLERR)){
        if(errorCallBack_) errorCallBack_();
    }
    if(revents_ & (EPOLLIN | EPOLLPRI)){
        if(readCallBack_) readCallBack_();
    }
    if(revents_ & EPOLLOUT){
        if(writeCallBack_) writeCallBack_();
    }
}

void Channel::remove()
{
    assert(isNoneEvent());
    loop_->removeChannel(this);
}
