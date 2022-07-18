#include "logger/logger.h"

int main() {
    Logger::get_instance()->init();
    LOG_DEBUG("%s", "LOG_DEBUG");
    LOG_INFO("%s", "LOG_INFO");
    LOG_WARN("%s", "LOG_WARN");
    LOG_ERR("%s", "LOG_ERR");
    LOG_DEBUG("%s", "中文 okay 吗？");
    return 0;
}
