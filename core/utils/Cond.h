//
// Created by song on 7/18/2022.
//

#ifndef SERVER_COND_H
#define SERVER_COND_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>


class Cond {
public:
    Cond() {
        if (pthread_cond_init(&cond_, nullptr) != 0) {
            throw std::exception();
        }
    }

    ~Cond() {
        pthread_cond_destroy(&cond_);
    }

    bool wait(pthread_mutex_t *mutex) {
        return pthread_cond_wait(&cond_, mutex) == 0;
    }

    bool time_wait(pthread_mutex_t *mutex, struct timespec t) {
        return pthread_cond_timedwait(&cond_, mutex, &t) == 0;
    }

    bool signal() {
        return pthread_cond_signal(&cond_) == 0;
    }

    bool broadcast() {
        return pthread_cond_broadcast(&cond_) == 0;
    }

private:
    pthread_cond_t cond_{};
};

#endif //SERVER_COND_H
