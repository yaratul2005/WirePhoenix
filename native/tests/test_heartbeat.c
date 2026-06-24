#include "pw_heartbeat.h"
#include <stdio.h>
#include <assert.h>

void test_heartbeat() {
    pw_heartbeat_t hb;
    pw_heartbeat_init(&hb, 5000, 15000); // ping every 5s, timeout after 15s

    // Initial state
    uint64_t current_time = 1000;
    pw_heartbeat_on_recv(&hb, current_time);
    pw_heartbeat_on_send(&hb, current_time);

    // Check at 3 seconds (no ping needed, not timed out)
    assert(pw_heartbeat_should_ping(&hb, current_time + 3000) == false);
    assert(pw_heartbeat_is_timed_out(&hb, current_time + 3000) == false);

    // Check at 6 seconds (ping needed, not timed out)
    assert(pw_heartbeat_should_ping(&hb, current_time + 6000) == true);
    assert(pw_heartbeat_is_timed_out(&hb, current_time + 6000) == false);

    // Simulate sending ping
    pw_heartbeat_on_ping_sent(&hb, current_time + 6000);
    assert(hb.waiting_for_pong == true);
    assert(pw_heartbeat_should_ping(&hb, current_time + 7000) == false); // shouldn't ping again while waiting

    // Simulate receiving pong
    pw_heartbeat_on_pong(&hb, current_time + 6500);
    assert(hb.waiting_for_pong == false);

    // Check at 20 seconds (timed out if we didn't receive anything since 6500, so last_recv_time + 15000 = 6500+15000 = 21500)
    // Actually current_time is 6500. So current_time + 6500 + 15000 = 28000. Wait, current_time in previous step was 1000+6500=7500.
    // Let's just use absolute times.

    current_time = 7500;
    pw_heartbeat_on_recv(&hb, current_time);

    assert(pw_heartbeat_is_timed_out(&hb, current_time + 14999) == false);
    assert(pw_heartbeat_is_timed_out(&hb, current_time + 15000) == true);

    printf("Heartbeat tests passed.\n");
}

int main() {
    test_heartbeat();
    return 0;
}
