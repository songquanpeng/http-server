//
// Created by song on 7/20/2022.
//

#ifndef SERVER_POLLER_H
#define SERVER_POLLER_H

#include <vector>
#include <map>
#include <poll.h>

class EventLoop;

class Channel;

class Poller {
public:
    typedef std::vector<Channel *> ChannelList;

    explicit Poller(EventLoop *loop);


    /// Poll I/O events
    /// Must be called in loop thread
    void poll(int timeoutMs, ChannelList *activeChannels);

    /// Change interested I/O events
    /// Must be called in loop thread
    void updateChannel(Channel *channel);

    /// Remove channel
    /// Must be called in loop thread
    void removeChannel(Channel *channel);

private:
    typedef std::vector<struct pollfd> PollFdList;
    typedef std::map<int, Channel *> ChannelMap;

    EventLoop *ownerLoop;
    PollFdList pollFdList;
    // Use channel's fd as key
    // Don't use unordered_map here
    ChannelMap channelMap;
};


#endif //SERVER_POLLER_H
