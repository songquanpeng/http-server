//
// Created by song on 7/23/2022.
//

#ifndef SERVER_EVENTLOOPTHREADPOOL_H
#define SERVER_EVENTLOOPTHREADPOOL_H

#include "EventLoop.h"
#include "EventLoopThread.h"

class EventLoopThreadPool {
public:
    EventLoopThreadPool(EventLoop *baseLoop, int numThreads);

    EventLoop *getNextLoop();

    void start();

private:

    EventLoop *baseLoop_;
    std::vector<std::shared_ptr<EventLoopThread>> threads;
    std::vector<EventLoop*> loops;
    int numThreads_ = 0;
    int nextLoopIdx = 0;
};


#endif //SERVER_EVENTLOOPTHREADPOOL_H
