//
// Created by song on 7/18/2022.
//

#ifndef SERVER_LOGGER_H
#define SERVER_LOGGER_H

#include <cstdarg>
#include <string>
#include <cstring>
#include <cstdio>
#include "../utils/BlockQueue.h"

class Logger {
public:
    static Logger *get_instance() {
        static Logger instance;
        return &instance;
    }

    void write(const char *file, int line, int level, const char *format, ...);

    static void *flush_log(void *args) {
        Logger::get_instance()->async_write();
    }

    void flush(void);

private:
    Logger() {
        init();
    };

    bool init(int buf_size = 8192, int max_queue_size = 0);

    virtual ~Logger() = default;

    void *async_write();


private:
    std::string save_path_;
    std::string filename_;
    int max_lines_;
    long long num_lines_;
    int buf_size_;
    char *buf_;
    bool is_async_;
    int today_;
    BlockQueue<std::string> *log_queue_;
    Mutex mutex_;
    FILE *fp_;
};

#define LOG_DEBUG(format, ...) {Logger::get_instance()->write(__FILE__, __LINE__, 0, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#define LOG_INFO(format, ...) {Logger::get_instance()->write(__FILE__, __LINE__, 1, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#define LOG_WARN(format, ...) {Logger::get_instance()->write(__FILE__, __LINE__, 2, format, ##__VA_ARGS__); Logger::get_instance()->flush();}
#define LOG_ERR(format, ...) {Logger::get_instance()->write(__FILE__, __LINE__, 3, format, ##__VA_ARGS__); Logger::get_instance()->flush();}


#endif //SERVER_LOGGER_H
