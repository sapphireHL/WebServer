#include "Poller.h"
#include "Channel.h"
#include <unistd.h>
#include <assert.h>
#include <string.h>

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

Poller::Poller(EventLoop* loop):
    loop_(loop),
    epollfd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
    //ctor
}

Poller::~Poller()
{
    //dtor
    close(epollfd_);
}

void Poller::poll(int timeout, ChannelList* activeChannels)
{
    int numEvents = epoll_wait(epollfd_, &*events_.begin(), events_.size(), timeout);

    if(numEvents > 0)
    {
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {
        //nothing
    }
    else
    {
        //error
        printf("epoll error\n");
        printf("errno:%d\n", errno);
    }
}

void Poller::updateChannel(Channel* channel)
{
    const int index = channel->index();
    if(index == kNew || index == kDeleted)
    {
        int fd = channel->fd();
        if(index == kNew)
        {
            //core dump?
            //assert(channels_.find(fd) == channels_.end());
            channels_[fd] = channel;
        }
        else
        {
            //assert(channels_.find(fd) != channels_.end());
            //assert(channels_[fd] == channel);
        }
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        int fd = channel->fd();
        assert(channels_.find(fd) != channels_.end());
        assert(channels_[fd] == channel);
        assert(index == kAdded);
        if(channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void Poller::removeChannel(Channel* channel)
{
    int fd = channel->fd();
    assert(channels_.find(fd) != channels_.end());
    assert(channels_[fd] == channel);
    assert(channel->isNoneEvent());
    int index = channel->index();
    assert(index == kAdded || index == kDeleted);
    channels_.erase(fd);
    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    //!!!!!!!!!!
    close(fd);
    channel->set_index(kNew);
}

void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels)
{
    for(int i=0; i<numEvents; ++i)
    {
        Channel* channel = static_cast<Channel*>(events_[i].data.ptr);

        int fd = channel->fd();
        ChannelMap::iterator it = channels_.find(fd);
        assert(it != channels_.end());
        assert(it->second == channel);

        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void Poller::update(int operation, Channel* channel)
{
    struct epoll_event event;
    bzero(&event, sizeof(event));
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    epoll_ctl(epollfd_, operation, fd, &event);
}

