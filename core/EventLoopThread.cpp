//
// Created by song on 7/23/2022.
//

#include <cassert>
#include "EventLoopThread.h"

EventLoopThread::EventLoopThread() : loop_(nullptr), mutex_(), cond_(), pid_(-1), started_(false) {

}

EventLoopThread::~EventLoopThread() {
    if (pid_ != -1) {
        // Wait for thread to end
        // TODO: pthread_join & pthread_detach
        pthread_join(pid_, nullptr);
    }
}


EventLoop *EventLoopThread::startLoop() {
    if (started_) {
        LOG_FATAL("EventLoopThread::startLoop trying to start a started event loop_ thread");
    }
    started_ = true;
    pthread_t tid;
    if (pthread_create(&tid, nullptr, threadFunc, this)) {
        LOG_FATAL("EventLoopThread::startLoop failed to create thread");
    }
    {
        MutexGuard lock(mutex_);
        while (loop_ == nullptr) {
            cond_.wait(mutex_.get());
        }
    }
    return loop_;
}

void *EventLoopThread::threadFunc(void *obj) {
    auto thread = (EventLoopThread *) obj;
    thread->start();
    return nullptr;
}

void EventLoopThread::start() {
    EventLoop localLoop;
    {
        MutexGuard lock(mutex_);
        loop_ = &localLoop;
        cond_.signal();
    }
    localLoop.loop();
}


