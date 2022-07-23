//
// Created by song on 7/23/2022.
//

#include "EventLoopThreadPool.h"
#include "EventLoop.h"

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop, int numThreads) : baseLoop_(baseLoop),
                                                                                numThreads_(numThreads) {

}

void EventLoopThreadPool::start() {
    LOG_INFO("EventLoopThreadPool::start creating %d threads", numThreads_);
    baseLoop_->assertInLoopThread();
    for (int i = 0; i < numThreads_; i++) {
        auto thread = new EventLoopThread;
        // TODO: why emplace_back work & make_shared
//        threads.push_back(std::make_shared<EventLoopThread>());
        threads.emplace_back(thread);
        loops.push_back(thread->startLoop());
    }
}


EventLoop *EventLoopThreadPool::getNextLoop() {
    baseLoop_->assertInLoopThread();
    auto loop = baseLoop_;
    if (!loops.empty()) {
        loop = loops[nextLoopIdx];
        nextLoopIdx++;
        if (nextLoopIdx >= loops.size()) {
            nextLoopIdx = 0;
        }
    }
    return loop;
}

