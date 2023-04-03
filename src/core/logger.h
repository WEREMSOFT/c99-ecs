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
        char prefix[80]; \
        sprintf(prefix, GREEN "[%02d:%02d:%02d] %s:%d", \
               tm.tm_hour, tm.tm_min, tm.tm_sec, \
               __FILE__, __LINE__); \
        int prefix_length = strlen(prefix); \
        printf("%s", prefix); \
        printf("%*s:", (int)(40 - prefix_length - strlen(RESET)), ""); \
        printf(format, ##__VA_ARGS__); \
        printf(RESET "\n"); \
    } while (0)

#define loggerError(format, ...) \
 	do { \
        time_t t = time(NULL); \
        struct tm tm = *localtime(&t); \
        char prefix[80]; \
        sprintf(prefix, RED "[%02d:%02d:%02d] %s:%d", \
               tm.tm_hour, tm.tm_min, tm.tm_sec, \
               __FILE__, __LINE__); \
        int prefix_length = strlen(prefix); \
        printf("%s", prefix); \
        printf("%*s:", (int)(40 - prefix_length - strlen(RESET)), ""); \
        printf(format, ##__VA_ARGS__); \
        printf(RESET "\n"); \
    } while (0)

#define loggerWarning(format, ...) \
 	do { \
        time_t t = time(NULL); \
        struct tm tm = *localtime(&t); \
        char prefix[80]; \
        sprintf(prefix, YELLOW "[%02d:%02d:%02d] %s:%d", \
               tm.tm_hour, tm.tm_min, tm.tm_sec, \
               __FILE__, __LINE__); \
        int prefix_length = strlen(prefix); \
        printf("%s", prefix); \
        printf("%*s:", (int)(40 - prefix_length - strlen(RESET)), ""); \
        printf(format, ##__VA_ARGS__); \
        printf(RESET "\n"); \
    } while (0)
#endif