#pragma once
#include <memory>
#include <sstream>
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

class Logger {
    static inline std::unique_ptr<LoggerBase> i =
            std::make_unique<utility::ConsoleLogger>();
public:
    static void set_implementation(std::unique_ptr<utility::LoggerBase> ptr) {
        i.reset();
        i = std::move(ptr);
    }
    template<class... Args>
    static void warn(Args&&... args) {
        if (Logger::i) {
            std::stringstream s;
            (s << ... <<args);
            warn(s.str().data());
        }
    }
    template<class... Args>
    static void info(Args&&... args) {
        if (Logger::i) {
            std::stringstream s;
            (s << ... <<args);
            info(s.str().data());
        }
    }

    template<class... Args>
    static void critical(Args&&... args) {
        if (Logger::i) {
            std::stringstream s;
            (s << ... <<args);
            critical(s.str().data());
        }
    }
    static void warn(const char * msg) {
        if (Logger::i)
            Logger::i->warn(msg);
    }

    static void info(const char * msg) {
        if (Logger::i)
            Logger::i->info(msg);
    }
    static void critical(const char * msg) {
        if (Logger::i)
            Logger::i->critical(msg);
    }
};

} // namespace utility
} // namespace telegram
