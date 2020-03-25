#pragma once
#include "traits.h"
namespace telegram {

namespace utility {
template <typename T,std::enable_if_t<traits::is_logger_v<T>>>
class Logger {
    static T* m_logger;
public:
    Logger(Logger&& ) =delete;
    Logger(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    static void setLog(T&& logger) {
        if (m_logger != nullptr)
            m_logger = logger;
    }
    static void warn(const char * info) {
        if (m_logger != nullptr)
            m_logger->warn(info);
    }
    static void info(const char * info) {
        if (m_logger != nullptr)
            m_logger->info(info);
    }
    static void critical(const char * info) {
        if (m_logger != nullptr)
            m_logger->critical(info);
    }
    ~Logger() {
        delete m_logger;
    }
};
}
}
