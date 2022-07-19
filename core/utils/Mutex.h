//
// Created by song on 7/18/2022.
//

#ifndef SERVER_MUTEX_H
#define SERVER_MUTEX_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>

class Mutex {
public:
    Mutex() {
        if (pthread_mutex_init(&mutex_, nullptr) != 0) {
            throw std::exception();
        }
    }

    ~Mutex() {
        pthread_mutex_destroy(&mutex_);
    }

    bool lock() {
        return pthread_mutex_lock(&mutex_) == 0;
    }

    bool unlock() {
        return pthread_mutex_unlock(&mutex_) == 0;
    }

    pthread_mutex_t *get() {
        return &mutex_;
    }

private:
    pthread_mutex_t mutex_{};
};


#endif //SERVER_MUTEX_H
