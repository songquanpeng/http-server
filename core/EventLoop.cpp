//
// Created by song on 7/19/2022.
//

#include <csignal>
#include <sys/eventfd.h>
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

int createEventFd() {
    int fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (fd < 0) {
        LOG_FATAL("EventLoop::EventLoop failed in eventfd");
    }
    return fd;
}

EventLoop::EventLoop() : threadId(gettid()), poller(new Poller(this)), wakeUpFd_(createEventFd()),
                         wakeUpChannel_(new Channel(this, wakeUpFd_)) {
    LOG_INFO("EventLoop %d created in thread %d", this, threadId);
    if (eventLoopOfThisThread) {
        LOG_FATAL("Another EventLoop already created in this thread %d", threadId);
    } else {
        eventLoopOfThisThread = this;
    }
    int fd = wakeUpFd_;
    wakeUpChannel_->setReadCallback([fd]() -> void {
        uint64_t one = 1;
        read(fd, &one, sizeof one);
    });
    wakeUpChannel_->enableListeningReadEvent();
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
    if (!isInLoopThread() || isCallingPendingFunctors) {
        wakeUp();
    }
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

void EventLoop::removeChannel(Channel *channel) {
    poller->removeChannel(channel);
}

// If we don't have  wakeUp(), the event loop will have to wait timeout to start processing pending factors.
void EventLoop::wakeUp() const {
    uint64_t one = 1;
    write(wakeUpFd_, &one, sizeof one);
}
