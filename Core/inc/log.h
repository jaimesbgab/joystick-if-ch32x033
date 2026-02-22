#ifndef LOG_H
#define LOG_H

#include <string.h>
#include "xprintf.h"

// ANSI color codes
#define COLOR_RESET   "\033[0m"
#define COLOR_INFO    "\033[32m"  // Green
#define COLOR_WARN    "\033[33m"  // Yellow
#define COLOR_ERROR   "\033[31m"  // Red
#define COLOR_DEBUG   "\033[36m"  // Cyan

#ifdef _WIN32
#define __FILENAME__ ( (const char*) ( __FILE__ + ( \
    sizeof(__FILE__) - 1 - ( \
        (__builtin_strrchr(__FILE__, '\\') ? (sizeof(__FILE__) - 1 - (__builtin_strrchr(__FILE__, '\\') - __FILE__) - 1) : 0) \
    ) \
)))
#else
#define __FILENAME__ ( (const char*) ( __FILE__ + ( \
    sizeof(__FILE__) - 1 - ( \
        (__builtin_strrchr(__FILE__, '/') ? (sizeof(__FILE__) - 1 - (__builtin_strrchr(__FILE__, '/') - __FILE__) - 1) : 0) \
    ) \
)))
#endif

// Core log macro
#define LOG_PRINT(color, level, fmt, ...) \
    xprintf(color "[%s:%d] " level ": " COLOR_RESET fmt "\n\r", \
      __FILENAME__, __LINE__, ##__VA_ARGS__)

// Log level macros
#define LOG_INFO(fmt, ...)  LOG_PRINT(COLOR_INFO,  "INFO",  fmt, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...)  LOG_PRINT(COLOR_WARN,  "WARN",  fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG_PRINT(COLOR_ERROR, "ERROR", fmt, ##__VA_ARGS__)
#define LOG_DEBUG(fmt, ...) LOG_PRINT(COLOR_DEBUG, "DEBUG", fmt, ##__VA_ARGS__)
#define LOG_TRACE(fmt, ...) LOG_PRINT(COLOR_RESET, "TRACE", fmt, ##__VA_ARGS__)

#endif // LOG_H
