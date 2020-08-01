#pragma once
#include <memory>
#include <sstream>
#include <spdlog/spdlog.h>
namespace telegram {

namespace utility {
class LoggerBase {
public:
  virtual void warn(const char *info) = 0;
  virtual void info(const char *info) = 0;
  virtual void critical(const char *info) = 0;
  virtual ~LoggerBase() {}
};

// simple logger is used by default
class ConsoleLogger : public LoggerBase{
public:
    void warn(const char *info) override {
        printf("\n[WARNING]: %s",info);
    }
    void info(const char *info) override {
        printf("\n[INFO]: %s",info);
    }
    void critical(const char *info) override {
        printf("\n[CRITICAL]: %s",info);
    }
};
class SpdLogger : public LoggerBase {
public:
    void warn(const char *info) override {
        spdlog::warn(info);
    }
    void info(const char *info) override {
        spdlog::info(info);
    }
    void critical(const char *info) override {
        spdlog::critical(info);
    }
};

class Logger {
    static inline std::unique_ptr<LoggerBase> i =
            std::make_unique<utility::SpdLogger>();
public:
    static void set_implementation(std::unique_ptr<utility::LoggerBase> ptr) {
        i.reset();
        i = std::move(ptr);
    }
    template<class... Args>
    static void warn(Args&&... args) {
#if TGLIB_VERBOSITY_LEVEL > 0
        if (Logger::i) {
            std::stringstream s;
            (s << ... <<args);
            i->warn(s.str().data());
        }
#endif
    }
    template<class... Args>
    static void info(Args&&... args) {
#if TGLIB_VERBOSITY_LEVEL > 1
        if (Logger::i) {
            std::stringstream s;
            (s << ... <<args);
            i->info(s.str().data());
        }
#endif
    }

    template<class... Args>
    static void critical(Args&&... args) {
#if TGLIB_VERBOSITY_LEVEL > 0
        if (Logger::i) {
            std::stringstream s;
            (s << ... <<args);
            i->critical(s.str().data());
        }
#endif
    }
};

} // namespace utility
} // namespace telegram
