//
// Created by song on 7/19/2022.
//

#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include <functional>
#include <pthread.h>

#include "logger/Logger.h"

class EventLoop {
public:
    typedef std::function<void()> Functor;

    EventLoop();

    ~EventLoop();

    /// Call by user
    /// The main part is a while loop.
    /// In each loop, we use select() / poll() / epoll() to check which fd have interested events happens.
    /// Then call the corresponding handlers.
    /// Finally do the jobs that passed in by queueInLoop().
    void loop();

    bool isInLoopThread() const;

    void assertInLoopThread();

    void runInLoop(const Functor &cb);

    void queueInLoop(const Functor &cb);

};


#endif //SERVER_EVENTLOOP_H
