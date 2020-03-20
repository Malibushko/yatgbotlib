#pragma once
#include "traits.h"
namespace telegram {
namespace utility {
template <class T,std::enable_if_t<traits::is_lo>
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
}
