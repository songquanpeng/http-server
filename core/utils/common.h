//
// Created by song on 7/21/2022.
//

#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

__thread pid_t t_cachedTid = 0;


pid_t gettid() {
    if (t_cachedTid == 0) {
        t_cachedTid = syscall(__NR_gettid);
    }
    return t_cachedTid;
}

#endif //SERVER_COMMON_H
