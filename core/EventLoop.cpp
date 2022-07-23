//
// Created by song on 7/19/2022.
//

#include <csignal>
#include "EventLoop.h"
#include "Mutex.h"
#include "Channel.h"
#include "common.h"

__thread EventLoop *eventLoopOfThisThread = nullptr;
const int kPollTimeMs = 10000;

class IgnoreSigPipe {
public:
    // TODO: SIGPIPE, SIG_IGN
    IgnoreSigPipe() {
        signal(SIGPIPE, SIG_IGN); // In case the client close the connection when we are still writing.
    }
};

IgnoreSigPipe ignoreSigPipe;

EventLoop::EventLoop() : threadId(gettid()), poller(new Poller(this)) {
    LOG_INFO("EventLoop %d created in thread %d", this, threadId);
    if (eventLoopOfThisThread) {
        LOG_FATAL("Another EventLoop already created in this thread %d", threadId);
    } else {
        eventLoopOfThisThread = this;
    }
}

EventLoop::~EventLoop() {
    eventLoopOfThisThread = nullptr;
    LOG_INFO("EventLoop %d destroyed in thread %d", this, threadId);
}

void EventLoop::loop() {
    assertInLoopThread();
    while (!isQuited) {
        activeChannels.clear();
        poller->poll(kPollTimeMs, &activeChannels);
        for (auto channel: activeChannels) {
            channel->handleHappenedEvents();
        }
        doPendingFactors();
    }
    LOG_INFO("EventLoop %d stopped looping", this);
}

bool EventLoop::isInLoopThread() const {
    return threadId == gettid();
}

void EventLoop::assertInLoopThread() const {
    if (!isInLoopThread()) {
        LOG_FATAL("assertInLoopThread failed, should in thread %d but in %d", threadId, gettid());
    }
}

void EventLoop::runInLoop(const EventLoop::Functor &cb) {
    if (isInLoopThread()) {
        cb();
    } else {
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const EventLoop::Functor &cb) {
    {
        MutexGuard lock(mutex);
        pendingFunctors.push_back(cb);
    }
    // TODO: wake up event loop to run functor ASAP
}

void EventLoop::doPendingFactors() {
    std::vector<Functor> functors;
    isCallingPendingFunctors = true;
    {
        MutexGuard lock(mutex);
        std::swap(functors, pendingFunctors);
    }
    for (auto &functor: functors) {
        functor();
    }
    isCallingPendingFunctors = false;
}

void EventLoop::updateChannel(Channel *channel) {
    poller->updateChannel(channel);
}
