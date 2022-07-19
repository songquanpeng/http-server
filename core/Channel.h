//
// Created by song on 7/19/2022.
//

#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include <functional>

class Channel {
public:
    typedef std::function<void()> EventCallback;

    Channel();

    ~Channel();

    void setReadCallback(const EventCallback &cb);

    void setWriteCallback(const EventCallback &cb);

    void setErrorCallback(const EventCallback &cb);

    void setCloseCallback(const EventCallback &cb);

    /// Call by poller
    void getFd() const;

    /// Call by poller
    void getListeningEvents() const;

    void enableListeningReadEvent();

    void enableListeningWriteEvent();

    void disableListeningWriteEvent();

    void disableAllEvent();

    bool isListeningWriteEvent() const;

    /// Call by poller
    void setHappenedEvents() const;

    /// Call by EventLoop::loop()
    void handleHappenedEvents();

private:
    // Tell EventLoop my listening events changed.
    void notifyEventLoop();
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;
};


#endif //SERVER_CHANNEL_H
