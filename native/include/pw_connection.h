#ifndef PW_CONNECTION_H
#define PW_CONNECTION_H

#include "phoenixwire.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    pw_connection_state_t state;
    /* Other fields (fd, buffers, etc.) will be added as transport I/O is implemented */
} pw_connection_t;

/**
 * Initializes a new connection structure.
 */
void pw_connection_init(pw_connection_t* conn);

/**
 * Attempts to transition the connection to a new state.
 * Returns true if successful, false if the transition is invalid.
 */
bool pw_connection_transition(pw_connection_t* conn, pw_connection_state_t new_state);

#ifdef __cplusplus
}
#endif

#endif /* PW_CONNECTION_H */
