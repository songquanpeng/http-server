//
// Created by song on 7/19/2022.
//

#include "Channel.h"
#include "EventLoop.h"

// TODO: poll events
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;
const int Channel::kNoneEvent = 0;

Channel::Channel(EventLoop *loop, int fd) : ownerLoop(loop), fd_(fd) {

}

Channel::~Channel() {

}

void Channel::setReadCallback(const Channel::EventCallback &cb) {
    readCallback = cb;
}

void Channel::setWriteCallback(const Channel::EventCallback &cb) {
    writeCallback = cb;
}

void Channel::setErrorCallback(const Channel::EventCallback &cb) {
    errorCallback = cb;
}

void Channel::setCloseCallback(const Channel::EventCallback &cb) {
    closeCallback = cb;
}

int Channel::getFd() const {
    return fd_;
}

short Channel::getListeningEvents() const {
    return (short) listeningEvents_;
}

void Channel::enableListeningReadEvent() {
    listeningEvents_ |= kReadEvent;
    ownerLoop->updateChannel(this);
}

void Channel::enableListeningWriteEvent() {
    listeningEvents_ |= kWriteEvent;
    ownerLoop->updateChannel(this);
}

void Channel::disableListeningWriteEvent() {
    listeningEvents_ &= ~kWriteEvent;
    ownerLoop->updateChannel(this);
}

void Channel::disableListeningAllEvent() {
    listeningEvents_ = kNoneEvent;
    ownerLoop->updateChannel(this);
}

bool Channel::isListeningWriteEvent() const {
    return listeningEvents_ & kWriteEvent;
}

void Channel::setHappenedEvents(int happenedEvents) {
    happenedEvents_ = happenedEvents;
}

void Channel::handleHappenedEvents() {
    handlingEvents = true;
    if ((happenedEvents_ & POLLHUP) && !(happenedEvents_ & POLLIN)) {
        if (closeCallback) closeCallback();
    }
    if (happenedEvents_ & (POLLERR | POLLNVAL)) {
        if (errorCallback) errorCallback();
    }
    if (happenedEvents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
        if (readCallback) readCallback();
    }
    if (happenedEvents_ & POLLOUT) {
        if (writeCallback) writeCallback();
    }
    handlingEvents = false;
}

int Channel::getIndex() const {
    return index_;
}

void Channel::setIndex(int index) {
    index_ = index;
}

bool Channel::isWriting() const {
    return listeningEvents_ & kWriteEvent;
}

