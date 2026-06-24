#include "pw_heartbeat.h"

void pw_heartbeat_init(pw_heartbeat_t* hb, uint64_t ping_interval_ms, uint64_t timeout_ms) {
    if (hb) {
        hb->last_recv_time = 0;
        hb->last_send_time = 0;
        hb->ping_interval_ms = ping_interval_ms;
        hb->timeout_ms = timeout_ms;
        hb->waiting_for_pong = false;
    }
}

void pw_heartbeat_on_recv(pw_heartbeat_t* hb, uint64_t current_time_ms) {
    if (hb) {
        hb->last_recv_time = current_time_ms;
    }
}

void pw_heartbeat_on_send(pw_heartbeat_t* hb, uint64_t current_time_ms) {
    if (hb) {
        hb->last_send_time = current_time_ms;
    }
}

bool pw_heartbeat_should_ping(const pw_heartbeat_t* hb, uint64_t current_time_ms) {
    if (!hb || hb->ping_interval_ms == 0) return false;

    /* Don't send another ping if we're already waiting for a pong */
    if (hb->waiting_for_pong) return false;

    /* We ping if it's been longer than ping_interval_ms since we last received data */
    return (current_time_ms >= hb->last_recv_time + hb->ping_interval_ms);
}

bool pw_heartbeat_is_timed_out(const pw_heartbeat_t* hb, uint64_t current_time_ms) {
    if (!hb || hb->timeout_ms == 0) return false;

    return (current_time_ms >= hb->last_recv_time + hb->timeout_ms);
}

void pw_heartbeat_on_pong(pw_heartbeat_t* hb, uint64_t current_time_ms) {
    if (hb) {
        hb->waiting_for_pong = false;
        hb->last_recv_time = current_time_ms;
    }
}

void pw_heartbeat_on_ping_sent(pw_heartbeat_t* hb, uint64_t current_time_ms) {
    if (hb) {
        hb->waiting_for_pong = true;
        hb->last_send_time = current_time_ms;
    }
}
