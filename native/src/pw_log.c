#include "pw_log.h"
#include <stdio.h>

static pw_log_callback_t g_log_cb = NULL;
static pw_log_level_t g_log_level = PW_LOG_LEVEL_INFO;

void pw_log_set_callback(pw_log_callback_t cb) {
    g_log_cb = cb;
}

void pw_log_set_level(pw_log_level_t level) {
    g_log_level = level;
}

void pw_log_emit(pw_log_level_t level, const char* file, int line, const char* format, ...) {
    if (level < g_log_level) return;

    char buffer[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    if (g_log_cb) {
        g_log_cb(level, file, line, buffer);
    } else {
        const char* level_str = "UNK";
        switch (level) {
            case PW_LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
            case PW_LOG_LEVEL_INFO:  level_str = "INFO"; break;
            case PW_LOG_LEVEL_WARN:  level_str = "WARN"; break;
            case PW_LOG_LEVEL_ERROR: level_str = "ERROR"; break;
        }
        fprintf(stderr, "[%s] %s:%d: %s\n", level_str, file, line, buffer);
    }
}
