#include "pw_flow.h"

void pw_flow_init(pw_flow_control_t* flow, uint32_t max_outbound, uint32_t max_inbound) {
    if (flow) {
        flow->max_outbound_bytes = max_outbound;
        flow->current_outbound_bytes = 0;
        flow->max_inbound_bytes = max_inbound;
        flow->current_inbound_bytes = 0;
    }
}

bool pw_flow_can_send(const pw_flow_control_t* flow, uint32_t bytes_to_send) {
    if (!flow || flow->max_outbound_bytes == 0) return true; // 0 = unlimited
    return (flow->current_outbound_bytes + bytes_to_send <= flow->max_outbound_bytes);
}

void pw_flow_on_enqueued(pw_flow_control_t* flow, uint32_t bytes_enqueued) {
    if (flow) {
        flow->current_outbound_bytes += bytes_enqueued;
    }
}

void pw_flow_on_sent(pw_flow_control_t* flow, uint32_t bytes_sent) {
    if (flow) {
        if (flow->current_outbound_bytes >= bytes_sent) {
            flow->current_outbound_bytes -= bytes_sent;
        } else {
            flow->current_outbound_bytes = 0;
        }
    }
}

bool pw_flow_can_receive(const pw_flow_control_t* flow, uint32_t bytes_to_receive) {
    if (!flow || flow->max_inbound_bytes == 0) return true; // 0 = unlimited
    return (flow->current_inbound_bytes + bytes_to_receive <= flow->max_inbound_bytes);
}

void pw_flow_on_received(pw_flow_control_t* flow, uint32_t bytes_received) {
    if (flow) {
        flow->current_inbound_bytes += bytes_received;
    }
}

void pw_flow_on_consumed(pw_flow_control_t* flow, uint32_t bytes_consumed) {
    if (flow) {
        if (flow->current_inbound_bytes >= bytes_consumed) {
            flow->current_inbound_bytes -= bytes_consumed;
        } else {
            flow->current_inbound_bytes = 0;
        }
    }
}
