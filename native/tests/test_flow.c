#include "pw_flow.h"
#include <stdio.h>
#include <assert.h>

void test_flow_control() {
    pw_flow_control_t flow;
    pw_flow_init(&flow, 1024, 2048);

    assert(pw_flow_can_send(&flow, 500) == true);
    pw_flow_on_enqueued(&flow, 500);

    assert(pw_flow_can_send(&flow, 600) == false); // 500 + 600 > 1024

    pw_flow_on_sent(&flow, 200);
    assert(pw_flow_can_send(&flow, 600) == true); // 300 + 600 <= 1024


    assert(pw_flow_can_receive(&flow, 1000) == true);
    pw_flow_on_received(&flow, 1000);

    assert(pw_flow_can_receive(&flow, 1500) == false); // 1000 + 1500 > 2048

    pw_flow_on_consumed(&flow, 500);
    assert(pw_flow_can_receive(&flow, 1500) == true); // 500 + 1500 <= 2048

    printf("Flow control tests passed.\n");
}

int main() {
    test_flow_control();
    return 0;
}
