//
// Created by song on 7/19/2022.
//

#ifndef SERVER_BUFFER_H
#define SERVER_BUFFER_H

#include <string>
#include <vector>
#include <sys/uio.h>
#include "logger/Logger.h"

class Buffer {
public:
    static const size_t kInitialSize = 1024;

    Buffer() : buffer_(kInitialSize), readIdx_(0), writeIdx_(0) {

    }

    size_t readableBytes() const {
        return writeIdx_ - readIdx_;
    }

    size_t writableBytes() const {
        return buffer_.size() - writeIdx_;
    }

    const char *peek() const {
        return begin() + readIdx_;
    }

    void retrieve(size_t n) {
        readIdx_ += n;
    }

    void retrieveAll() {
        readIdx_ = 0;
        writeIdx_ = 0;
    }

    std::string retrieveAllAsString() {
        std::string data(peek(), readableBytes());
        retrieveAll();
        return data;
    }

    void append(const std::string &str) {
        append(str.data(), str.size());
    }

    void append(const void *data, size_t n) {
        append(static_cast<const char *>(data), n);
    }

    void append(const char *data, size_t n) {
        ensureAbleToWriteNBytes(n);
        std::copy(data, data + n, begin() + writeIdx_);
        // Don't forget to update the writeIdx_!
        writeIdx_ += n;
    }

    void ensureAbleToWriteNBytes(size_t n) {
        if (writableBytes() < n) {
            if (n + readableBytes() < buffer_.size()) {
                // The buffer have enough bytes, but it's separated.
                auto readableBytes_ = readableBytes();
                std::copy(begin() + readIdx_, begin() + writeIdx_, begin());
                readIdx_ = 0;
                writeIdx_ = readableBytes_;
            } else {
                // Otherwise we have to resize buffer_
                buffer_.resize(writeIdx_ + n);
            }
        }
    }

    ssize_t readFd(int fd) {
        // Please notice here, we do not allocate memory! We directly use the unused stack memory here.
        char extraBuf[65536];
        struct iovec vec[2];
        const size_t writable = writableBytes();
        vec[0].iov_base = begin() + writeIdx_;
        vec[0].iov_len = writable;
        vec[1].iov_base = extraBuf;
        vec[1].iov_len = sizeof extraBuf;
        auto n = readv(fd, vec, 2);
        if (n < 0) {
            LOG_ERROR("Buffer::readFd get %d as return", n);
        } else if (n <= writable) {
            // Able to read without use extraBuf;
            writeIdx_ += n;
        } else {
            writeIdx_ = buffer_.size();
            append(extraBuf, n - writable);
        }
        return n;
    }

private:
    std::vector<char> buffer_;
    size_t readIdx_;
    size_t writeIdx_;

    char *begin() {
        // TODO:  &*buffer_.begin(); ?
        return &*buffer_.begin();
    }

    // TODO: two const here
    const char *begin() const {
        return &*buffer_.begin();
    }

};


#endif //SERVER_BUFFER_H
