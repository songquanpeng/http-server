//
// Created by song on 7/18/2022.
//

#include "logger.h"
#include <sys/time.h>

using namespace std;

bool Logger::init(int buf_size, int max_queue_size) {
    if (max_queue_size >= 1) {  // async
        is_async_ = true;
        log_queue_ = new BlockQueue<string>(max_queue_size);
        pthread_t tid;
        pthread_create(&tid, nullptr, flush_log, nullptr);
    }
    buf_size_ = buf_size;
    buf_ = new char[buf_size_];
    memset(buf_, '\0', buf_size_);

    fp_ = fopen("server.log", "a");
    return fp_ != nullptr;
}

void Logger::write(const char *file, int line, int level, const char *format, ...) {
    struct timeval now = {0, 0};
    gettimeofday(&now, nullptr);
    time_t t = now.tv_sec;
    struct tm *sys_tm = localtime(&t);
    struct tm my_tm = *sys_tm;
    char s[16] = {0};
    switch (level) {
        case 0:
            strcpy(s, "[DEBUG]:");
            break;
        case 1:
            strcpy(s, "[INFO]:");
            break;
        case 2:
            strcpy(s, "[WARN]:");
            break;
        case 3:
            strcpy(s, "[ERR]:");
            break;
        default:
            strcpy(s, "[INFO]:");
            break;
    }
    mutex_.lock();
    num_lines_++;
    mutex_.unlock();

    va_list ap;
    va_start(ap, format);

    string log_str;
    mutex_.lock();

    int n = snprintf(buf_, 48, "%d-%02d-%02d %02d:%02d:%02d.%06ld %s ",
                     my_tm.tm_year + 1900, my_tm.tm_mon + 1, my_tm.tm_mday,
                     my_tm.tm_hour, my_tm.tm_min, my_tm.tm_sec, now.tv_usec, s);

    int m = vsnprintf(buf_ + n, buf_size_ - 200, format, ap);
    int k = snprintf(buf_ + n + m, buf_size_ - 1, " -- %s:%d", file, line);
    buf_[n + m + k] = '\n';
    buf_[n + m + k + 1] = '\0';
    log_str = buf_;

    mutex_.unlock();

    if (is_async_ && !log_queue_->full()) {
        log_queue_->push(log_str);
    } else {
        mutex_.lock();
        fputs(log_str.c_str(), fp_);
        mutex_.unlock();
    }
    va_end(ap);
#ifdef DEBUG
    setbuf(stdout, nullptr);
    printf("%s", log_str.c_str());
#endif
}

void *Logger::async_write() {
    string log;
    while (log_queue_->pop(log)) {
        mutex_.lock();
        fputs(log.c_str(), fp_);
        mutex_.unlock();
    }
}

void Logger::flush(void) {
    mutex_.lock();
    fflush(fp_);
    mutex_.unlock();
}

