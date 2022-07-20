//
// Created by song on 7/19/2022.
//

#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include <functional>
#include "EventLoop.h"
#include "Callbacks.h"

class Channel {
public:
    typedef std::function<void()> EventCallback;

    Channel(EventLoop *loop, int fd);

    ~Channel();

    void setReadCallback(const EventCallback &cb);

    void setWriteCallback(const EventCallback &cb);

    void setErrorCallback(const EventCallback &cb);

    void setCloseCallback(const EventCallback &cb);

    /// Call by poller
    int getFd() const;

    /// Call by poller
    short getListeningEvents() const;

    void enableListeningReadEvent();

    void enableListeningWriteEvent();

    void disableListeningWriteEvent();

    void disableAllEvent();

    bool isListeningWriteEvent() const;

    /// Call by poller
    void setHappenedEvents(int happenedEvents);

    /// Call by EventLoop::loop()
    void handleHappenedEvents();

    int getIndex() const;

    int setIndex(int index);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    int happenedEvents_ = 0;
    int listeningEvents_ = 0;
    int index_ = -1;
    bool handlingEvents = false;

    EventLoop* ownerLoop;
    const int fd_;

    EventCallback readCallback;
    EventCallback writeCallback;
    EventCallback errorCallback;
    EventCallback closeCallback;
};


#endif //SERVER_CHANNEL_H
