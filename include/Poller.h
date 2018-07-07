#ifndef POLLER_H
#define POLLER_H

#include <sys/epoll.h>
#include <vector>
#include <map>

class Channel;
class EventLoop;

class Poller
{
    public:
        typedef std::vector<Channel*> ChannelList;

        Poller(EventLoop* loop);
        ~Poller();

        void poll(int timeout, ChannelList* activeChannels);
        void updateChannel(Channel* channel);
        void removeChannel(Channel* channel);


    private:
        void update(int operation, Channel* channel);

        static const int kInitEventListSize = 16;

        void fillActiveChannels(int numEvents, ChannelList* activeChannels);

        typedef std::map<int, Channel*> ChannelMap;//fd to channel
        typedef std::vector<struct epoll_event> EventList;

        EventLoop* loop_;
        int epollfd_;
        EventList events_;
        ChannelMap channels_;
};

#endif // POLLER_H
