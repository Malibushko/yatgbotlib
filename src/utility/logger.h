#pragma once
#include <memory>
#include <sstream>
namespace telegram {

namespace utility {
class logger_base {
public:
  virtual void warn(const char *info) = 0;
  virtual void info(const char *info) = 0;
  virtual void critical(const char *info) = 0;
  virtual ~logger_base() {}
};

class logger {
    static inline std::unique_ptr<logger_base> i;
public:
    static void set_implementation(std::unique_ptr<utility::logger_base> ptr) {
        i.reset();
        i = std::move(ptr);
    }
    template<class... Args>
    static void warn(Args... args) {
        std::stringstream s;
        (s << ... <<args);
        warn(s.str());
    }
    template<class... Args>
    static void info(Args... args) {
        std::stringstream s;
        (s << ... <<args);
        info(s.str());
    }
    template<class... Args>
    static void critical(Args... args) {
        std::stringstream s;
        (s << ... <<args);
        critical(s.str());
    }
    static void warn(const char * msg) {
        if (logger::i)
            logger::i->warn(msg);
    }
    static void info(const char * msg) {
        if (logger::i)
            logger::i->info(msg);
    }
    static void critical(const char * msg) {
        if (logger::i)
            logger::i->critical(msg);
    }
};

} // namespace utility
} // namespace telegram
