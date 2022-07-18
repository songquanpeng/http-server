//
// Created by song on 7/18/2022.
//

#ifndef SERVER_BLOCK_QUEUE_H
#define SERVER_BLOCK_QUEUE_H

#include <iostream>
#include <cstdlib>
#include <pthread.h>
#include <sys/time.h>
#include "../utils/mutex.h"
#include "../utils/cond.h"

template<class T>
class BlockQueue {
public:
    explicit BlockQueue(int max_size = 1000) {
        if (max_size <= 0) {
            exit(-1);
        }
        max_size_ = max_size;
        array_ = new T[max_size];
        size_ = 0;
        front_ = -1;
        back_ = -1;
    }

    ~BlockQueue() {
        mutex_.lock();
        delete[] array_;
        mutex_.unlock();
    }

    bool push(const T &item) {
        mutex_.lock();
        if (size_ >= max_size_) {
            cond_.broadcast();
            mutex_.unlock();
            return false;
        }
        back_++;
        back_ %= max_size_;
        array_[back_] = item;
        size_++;
        cond_.broadcast();
        mutex_.unlock();
        return true;
    }

    bool pop(T &item) {
        mutex_.lock();
        while (size_ <= 0) {
            if (!cond_.wait(mutex_.get())) {
                mutex_.unlock();
                return false;
            }
        }
        front_++;
        front_ %= max_size_;
        item = array_[front_];
        size_--;
        mutex_.unlock();
        return true;
    }

    bool pop(T &item, int timeout_ms) {
        struct timespec t = {0, 0};
        struct timeval now = {0, 0};
        gettimeofday(&now, nullptr);
        mutex_.lock();
        if (size_ <= 0) {
            t.tv_sec = now.tv_sec + timeout_ms / 1000;
            t.tv_nsec = (timeout_ms % 1000) * 1000;
            if (!cond_.time_wait(mutex_.get(), t)) {
                mutex_.unlock();
                return false;
            }
        }

        if (size_ <= 0) {
            mutex_.unlock();
            return false;
        }

        front_++;
        front_ %= max_size_;
        item = array_[front_];
        size_--;
        mutex_.unlock();
        return true;
    }

    void clear() {
        mutex_.lock();
        size_ = 0;
        front_ = -1;
        back_ = -1;
        mutex_.unlock();
    }

    bool full() {
        mutex_.lock();
        if (size_ >= max_size_) {
            mutex_.unlock();
            return true;
        }
        mutex_.unlock();
        return false;
    }

    bool empty() {
        mutex_.lock();
        if (size_ == 0) {
            mutex_.unlock();
            return true;
        }
        mutex_.unlock();
        return false;
    }

    bool front(T &value) {
        mutex_.lock();
        if (size_ == 0) {
            mutex_.unlock();
            return false;
        }
        value = array_[front_];
        mutex_.unlock();
        return true;
    }

    bool back(T &value) {
        mutex_.lock();
        if (size_ == 0) {
            mutex_.unlock();
            return false;
        }
        value = array_[back_];
        mutex_.unlock();
        return true;
    }

    int size() {
        int tmp = 0;
        mutex_.lock();
        tmp = size_;
        mutex_.unlock();
        return tmp;
    }

    int max_size() {
        int tmp = 0;
        mutex_.lock();
        tmp = max_size_;
        mutex_.unlock();
        return tmp;
    }

private:
    Mutex mutex_;
    Cond cond_;

    T *array_;
    int size_{};
    int max_size_{};
    int front_{};
    int back_{};
};


#endif //SERVER_BLOCK_QUEUE_H
