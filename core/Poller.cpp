//
// Created by song on 7/20/2022.
//

#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"
#include "Logger.h"

#include <cassert>

Poller::Poller(EventLoop *loop) : ownerLoop(loop) {

}

void Poller::poll(int timeoutMs, Poller::ChannelList *activeChannels) {
    ownerLoop->assertInLoopThread();
    // https://man7.org/linux/man-pages/man2/poll.2.html
    // The call will block until either:
    // • a file descriptor becomes ready;
    // • the call is interrupted by a signal handler; or
    // • the timeout expires.
    int numEvents = ::poll(pollFdList.data(), pollFdList.size(), timeoutMs);
    if (numEvents > 0) {
        LOG_INFO("poll: %d events happened", numEvents);
        // Fill activeChannels
        for (auto iter = pollFdList.begin(); iter != pollFdList.end() && numEvents > 0; iter++) {
            if (iter->revents > 0) {
                numEvents--;
                auto targetIter = channelMap.find(iter->fd);
                assert(targetIter != channelMap.end());
                auto channel = targetIter->second;
                channel->setHappenedEvents(iter->revents);
                activeChannels->push_back(channel);
            }
        }
    } else if (numEvents < 0) {
        LOG_ERROR("poll return a negative value %d", numEvents);
    }
}

void Poller::updateChannel(Channel *channel) {
    ownerLoop->assertInLoopThread();
    if (channel->getIndex() < 0) {
        // This is a new channel
        struct pollfd pfd{};
        pfd.fd = channel->getFd();
        pfd.events = channel->getListeningEvents();
        pfd.revents = 0;
        pollFdList.push_back(pfd);
        int idx = (int) pollFdList.size() - 1;
        // The following line will cause Signal: SIGSEGV (Segmentation fault) on Release mode
        // The above bug has been solved, that's because the origin function definition is
        // --> int Channel::setIndex(int index)
        // But we don't return any value.
        // This is fixed by accident by checking for build warnings.
        channel->setIndex(idx);
        channelMap[pfd.fd] = channel;
        LOG_INFO("Poller::updateChannel new channel with fd %d added", pfd.fd);
    } else {
        // Update existing one
        int idx = channel->getIndex();
        auto &pfd = pollFdList[idx];
        pfd.events = channel->getListeningEvents();
        pfd.revents = 0;
        LOG_INFO("Poller::updateChannel channel with fd %d updated", pfd.fd);
        // TODO: ignore none event fd
    }
}

void Poller::removeChannel(Channel *channel) {
    ownerLoop->assertInLoopThread();
    int idx = channel->getIndex();
    auto &pfd = pollFdList[idx];
    // update channelMap
    channelMap.erase(channel->getFd());
    // update pollFdList
    if (idx != pollFdList.size() - 1) {
        // swap with the last one
        int lastChannelFd = pollFdList.back().fd;
        // update the origin last one's nextLoopIdx
        channelMap[lastChannelFd]->setIndex(idx);
        std::iter_swap(pollFdList.begin() + idx, pollFdList.end() - 1);
    }
    pollFdList.pop_back();
    LOG_INFO("Poller::updateChannel channel with fd %d removed", pfd.fd);
}
