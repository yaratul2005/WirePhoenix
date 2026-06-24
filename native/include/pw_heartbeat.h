#ifndef PW_HEARTBEAT_H
#define PW_HEARTBEAT_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint64_t last_recv_time;
    uint64_t last_send_time;
    uint64_t ping_interval_ms;
    uint64_t timeout_ms;
    bool waiting_for_pong;
} pw_heartbeat_t;

/**
 * Initializes heartbeat state.
 */
void pw_heartbeat_init(pw_heartbeat_t* hb, uint64_t ping_interval_ms, uint64_t timeout_ms);

/**
 * Updates the last receive time.
 */
void pw_heartbeat_on_recv(pw_heartbeat_t* hb, uint64_t current_time_ms);

/**
 * Updates the last send time.
 */
void pw_heartbeat_on_send(pw_heartbeat_t* hb, uint64_t current_time_ms);

/**
 * Checks if a ping should be sent.
 */
bool pw_heartbeat_should_ping(const pw_heartbeat_t* hb, uint64_t current_time_ms);

/**
 * Checks if the connection has timed out.
 */
bool pw_heartbeat_is_timed_out(const pw_heartbeat_t* hb, uint64_t current_time_ms);

/**
 * Call this when a pong is received.
 */
void pw_heartbeat_on_pong(pw_heartbeat_t* hb, uint64_t current_time_ms);

/**
 * Call this when a ping is sent.
 */
void pw_heartbeat_on_ping_sent(pw_heartbeat_t* hb, uint64_t current_time_ms);

#ifdef __cplusplus
}
#endif

#endif /* PW_HEARTBEAT_H */
