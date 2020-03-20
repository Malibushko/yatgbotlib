#pragma once
#include <utility>

namespace telegram {
template <class T>
class Logger {
    static T* m_logger;
public:
    Logger(Logger&& ) =delete;
    Logger(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    static void setLog(T&& logger) {
        m_logger = logger;
    }
};

}
