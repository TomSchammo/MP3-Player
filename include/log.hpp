#ifndef LOG_HPP
#define LOG_HPP

#include <experimental/source_location>
#include <fmt/core.h>
#include <config.h>


//    std::cout << "\033[31mred text\n" << std::endl;
//    std::cout << "\033[33myellow text\n" << std::endl;
//    std::cout << "\033[37mwhite text\n" << std::endl;
//    std::cout << "\033[34mblue text\n" << std::endl;
//    std::cout << "\033[0mdefault text\n" << std::endl;
// https://stackoverflow.com/questions/4053837/colorizing-text-in-the-console-with-c

class log {

public:
    static inline void info(const std::string& message, const std::experimental::source_location& location = std::experimental::source_location::current()) {
#ifdef LOG
        fmt::print("\033[37m[INFO]:[{}:{}]:[{}] {}\n",
                   location.file_name(), location.line(), location.function_name(), message);
#endif
    }


    static inline void debug(const std::string& message, const std::experimental::source_location& location = std::experimental::source_location::current()) {
#ifdef LOG
        fmt::print("\033[34m[DEBUG]:[{}:{}]:[{}] {}\n",
                   location.file_name(), location.line(), location.function_name(), message);
#endif
    }


    static inline void warn(const std::string& message, const std::experimental::source_location& location = std::experimental::source_location::current()) {
#ifdef LOG
        fmt::print("\033[33m[WARN]:[{}:{}]:[{}] {}\n",
                   location.file_name(), location.line(), location.function_name(), message);
#endif
    }


    static inline void error(const std::string& message, const std::experimental::source_location& location = std::experimental::source_location::current()) {
#ifdef LOG
        fmt::print("\033[31m[ERROR]:[{}:{}]:[{}] {}\n",
                   location.file_name(), location.line(), location.function_name(), message);
#endif
    }
};


#endif /* ifndef LOG_HPP */
