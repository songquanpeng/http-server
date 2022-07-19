//
// Created by song on 7/19/2022.
//

#include "EventLoop.h"


EventLoop::EventLoop() {

}

EventLoop::~EventLoop() {

}

void EventLoop::loop() {

}

bool EventLoop::isInLoopThread() const {
    return false;
}

void EventLoop::assertInLoopThread() {

}

void EventLoop::runInLoop(const EventLoop::Functor &cb) {

}

void EventLoop::queueInLoop(const EventLoop::Functor &cb) {

}
