#ifndef CHANNEL_H
#define CHANNEL_H

#include <functional>
#include "EventLoop.h"


class Channel
{
    public:
        typedef std::function<void()> EventCallBack;

        Channel(EventLoop* loop, int fd);
        ~Channel();

        void handleEvent();
        void setReadCallBack(EventCallBack cb) { readCallBack_ = cb; }
        void setWriteCallBack(EventCallBack cb) { writeCallBack_ = cb; }
        void setErrorCallBack(EventCallBack cb) { errorCallBack_ = cb; }
        void setCloseCallBack(EventCallBack cb) { closeCallBack_ = cb; }

        int fd() const { return fd_; }
        int events() const { return events_; }
        void set_revents(int revt) { revents_ = revt; }
        bool isNoneEvent() const { return events_ == kNoneEvent; }
        bool isWriting() { return events_ & kWriteEvent; }

        void enableReading() { events_ |= kReadEvent; update(); }
        void enableWriting() { events_ |= kWriteEvent; update(); }
        void disableWriting() { events_ &= ~kWriteEvent; update(); }
        void disableAll() { events_ = kNoneEvent; update(); }

        void remove();


        //for poller
        int index() const { return index_; }
        void set_index(int idx) { index_ = idx; }

        EventLoop* ownerLoop() { return loop_; }

        int events_;
        int revents_;

    private:
        void update();

        static const int kNoneEvent;
        static const int kReadEvent;
        static const int kWriteEvent;

        EventLoop* loop_;
        const int fd_;

        int index_;//used by poller

        EventCallBack readCallBack_;
        EventCallBack writeCallBack_;
        EventCallBack errorCallBack_;
        EventCallBack closeCallBack_;
};

#endif // CHANNEL_H
