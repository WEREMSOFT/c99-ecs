#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <stdio.h>
#include <time.h>

#define GREEN "\033[32m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define RESET "\033[0m"

#define loggerLog(format, ...) \
    do { \
        time_t t = time(NULL); \
        struct tm tm = *localtime(&t); \
        printf(GREEN "[%d-%02d-%02d %02d:%02d:%02d] " format "\n" RESET, \
               tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, \
               tm.tm_hour, tm.tm_min, tm.tm_sec, \
               ##__VA_ARGS__); \
    } while (0)

#define loggerError(format, ...) \
    do { \
        time_t t = time(NULL); \
        struct tm tm = *localtime(&t); \
        printf(RED "[%d-%02d-%02d %02d:%02d:%02d] " format "\n" RESET, \
               tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, \
               tm.tm_hour, tm.tm_min, tm.tm_sec, \
               ##__VA_ARGS__); \
    } while (0)

#define loggerWarning(format, ...) \
    do { \
        time_t t = time(NULL); \
        struct tm tm = *localtime(&t); \
        printf(YELLOW "[%d-%02d-%02d %02d:%02d:%02d] " format "\n" RESET, \
               tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, \
               tm.tm_hour, tm.tm_min, tm.tm_sec, \
               ##__VA_ARGS__); \
    } while (0)
#endif