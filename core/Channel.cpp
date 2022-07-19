//
// Created by song on 7/19/2022.
//

#include "Channel.h"

Channel::Channel() {

}

Channel::~Channel() {

}

void Channel::setReadCallback(const Channel::EventCallback &cb) {

}

void Channel::setWriteCallback(const Channel::EventCallback &cb) {

}

void Channel::setErrorCallback(const Channel::EventCallback &cb) {

}

void Channel::setCloseCallback(const Channel::EventCallback &cb) {

}

void Channel::getFd() const {

}

void Channel::getListeningEvents() const {

}

void Channel::enableListeningReadEvent() {

}

void Channel::enableListeningWriteEvent() {

}

void Channel::disableListeningWriteEvent() {

}

void Channel::disableAllEvent() {

}

bool Channel::isListeningWriteEvent() const {
    return false;
}

void Channel::setHappenedEvents() const {

}

void Channel::handleHappenedEvents() {

}

void Channel::notifyEventLoop() {

}
