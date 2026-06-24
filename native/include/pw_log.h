#ifndef PW_LOG_H
#define PW_LOG_H

#include <stdarg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PW_LOG_LEVEL_DEBUG = 0,
    PW_LOG_LEVEL_INFO = 1,
    PW_LOG_LEVEL_WARN = 2,
    PW_LOG_LEVEL_ERROR = 3
} pw_log_level_t;

typedef void (*pw_log_callback_t)(pw_log_level_t level, const char* file, int line, const char* msg);

void pw_log_set_callback(pw_log_callback_t cb);
void pw_log_set_level(pw_log_level_t level);

void pw_log_emit(pw_log_level_t level, const char* file, int line, const char* format, ...);

#define PW_LOG_DEBUG(...) pw_log_emit(PW_LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define PW_LOG_INFO(...)  pw_log_emit(PW_LOG_LEVEL_INFO,  __FILE__, __LINE__, __VA_ARGS__)
#define PW_LOG_WARN(...)  pw_log_emit(PW_LOG_LEVEL_WARN,  __FILE__, __LINE__, __VA_ARGS__)
#define PW_LOG_ERROR(...) pw_log_emit(PW_LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif /* PW_LOG_H */
