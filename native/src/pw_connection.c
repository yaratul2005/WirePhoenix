#include "pw_connection.h"

void pw_connection_init(pw_connection_t* conn) {
    if (conn) {
        conn->state = PW_STATE_DISCONNECTED;
    }
}

bool pw_connection_transition(pw_connection_t* conn, pw_connection_state_t new_state) {
    if (!conn) return false;

    pw_connection_state_t current = conn->state;
    bool valid = false;

    /* Implement strict transition checks based on lifecycle */
    switch (current) {
        case PW_STATE_DISCONNECTED:
            if (new_state == PW_STATE_CONNECTING) {
                valid = true;
            }
            break;

        case PW_STATE_CONNECTING:
            if (new_state == PW_STATE_HANDSHAKING || new_state == PW_STATE_CLOSED) {
                valid = true;
            }
            break;

        case PW_STATE_HANDSHAKING:
            if (new_state == PW_STATE_AUTHENTICATING || new_state == PW_STATE_CLOSED) {
                valid = true;
            }
            break;

        case PW_STATE_AUTHENTICATING:
            if (new_state == PW_STATE_READY || new_state == PW_STATE_CLOSED) {
                valid = true;
            }
            break;

        case PW_STATE_READY:
            if (new_state == PW_STATE_DRAINING || new_state == PW_STATE_CLOSING || new_state == PW_STATE_CLOSED) {
                valid = true;
            }
            break;

        case PW_STATE_DRAINING:
            if (new_state == PW_STATE_CLOSING || new_state == PW_STATE_CLOSED) {
                valid = true;
            }
            break;

        case PW_STATE_CLOSING:
            if (new_state == PW_STATE_CLOSED) {
                valid = true;
            }
            break;

        case PW_STATE_CLOSED:
            /* Terminal state, no further transitions allowed (unless re-initialized to DISCONNECTED, but we expect a new struct or explicit re-init) */
            if (new_state == PW_STATE_DISCONNECTED) {
                valid = true;
            }
            break;

        default:
            valid = false;
            break;
    }

    if (valid) {
        conn->state = new_state;
    }

    return valid;
}
