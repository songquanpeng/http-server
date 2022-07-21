//
// Created by song on 7/21/2022.
//

#ifndef SERVER_COMMON_H
#define SERVER_COMMON_H

#include <sys/types.h>
#include <unistd.h>
#include <sys/syscall.h>

int gettid() {
    return syscall(__NR_gettid);
}

#endif //SERVER_COMMON_H
