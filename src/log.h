#ifndef ACORN_LOG_H
#define ACORN_LOG_H

#include "core.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>

class Log {
public:
    /// Log an information message
    static void info(const char *format) {
        info("%s", format);
    }

    /// Log an information message with printf-like syntax
    template <typename ... Args>
    static void info(const char *format, const Args &... args) {
        fprintf(stdout, "[%s][info] ", utils::get_date_time_as_string().c_str());
        fprintf(stdout, format, args ...);
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    /// Log a debug message
    static void debug(const char *format) {
        debug("%s", format);
    }

    /// Log a debug message with printf-like syntax
    template <typename ... Args>
    static void debug(const char *format, const Args &... args) {
        if (!core->config.getConfigData().debugLoggingEnabled) {
            return;
        }

        fprintf(stdout, "[%s][debug] ", utils::get_date_time_as_string().c_str());
        fprintf(stdout, format, args ...);
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    /// Log a warning
    static void warn(const char *format) {
        warn("%s", format);
    }

    /// Log a warning with printf-like syntax
    template <typename ... Args>
    static void warn(const char *format, const Args &... args) {
        fprintf(stdout, "[%s][warn] ", utils::get_date_time_as_string().c_str());
        fprintf(stdout, format, args ...);
        fprintf(stdout, "\n");
        fflush(stdout);
    }

    /// Log a fatal error and exit
    [[noreturn]] static void fatal(const char *format) {
        fatal("%s", format);
    }

    /// Log a fatal error with printf-like syntax and exit
    template <typename ... Args>
    [[noreturn]] static void fatal(const char *format, const Args &... args) {
        fprintf(stderr, "[%s][fatal] ", utils::get_date_time_as_string().c_str());
        fprintf(stderr, format, args ...);
        fprintf(stderr, "\n");
        fflush(stderr);
        exit(1);
    }
};

#endif //ACORN_LOG_H
