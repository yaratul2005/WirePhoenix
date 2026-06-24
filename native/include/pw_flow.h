#ifndef PW_FLOW_H
#define PW_FLOW_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    uint32_t max_outbound_bytes;
    uint32_t current_outbound_bytes;

    uint32_t max_inbound_bytes;
    uint32_t current_inbound_bytes;

    /* Could add per-stream limits here as well, simplified for now */
} pw_flow_control_t;

/**
 * Initializes flow control limits.
 */
void pw_flow_init(pw_flow_control_t* flow, uint32_t max_outbound, uint32_t max_inbound);

/**
 * Checks if we can enqueue more data to send.
 */
bool pw_flow_can_send(const pw_flow_control_t* flow, uint32_t bytes_to_send);

/**
 * Records that data has been enqueued to send.
 */
void pw_flow_on_enqueued(pw_flow_control_t* flow, uint32_t bytes_enqueued);

/**
 * Records that data has been successfully sent out over the socket.
 */
void pw_flow_on_sent(pw_flow_control_t* flow, uint32_t bytes_sent);

/**
 * Checks if we can accept more incoming data (to prevent memory blowups).
 */
bool pw_flow_can_receive(const pw_flow_control_t* flow, uint32_t bytes_to_receive);

/**
 * Records that data has been received and buffered.
 */
void pw_flow_on_received(pw_flow_control_t* flow, uint32_t bytes_received);

/**
 * Records that data has been consumed by the application layer.
 */
void pw_flow_on_consumed(pw_flow_control_t* flow, uint32_t bytes_consumed);

#ifdef __cplusplus
}
#endif

#endif /* PW_FLOW_H */
