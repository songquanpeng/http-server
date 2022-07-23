//
// Created by song on 7/23/2022.
//

#ifndef SERVER_EVENTLOOPTHREAD_H
#define SERVER_EVENTLOOPTHREAD_H

#include "EventLoop.h"
#include "Cond.h"

class EventLoopThread {
public:
    EventLoopThread();

    ~EventLoopThread();

    EventLoop *startLoop();

private:
    void start();
    static void *threadFunc(void *obj);

    EventLoop *loop_;
    pid_t pid_;
    Mutex mutex_;
    Cond cond_;
    bool started_;
};


#endif //SERVER_EVENTLOOPTHREAD_H
