#include "pw_transport.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

/* Mock socket functions for testing would go here,
 * but testing real sockets requires setup.
 * We'll do a simple test of the buffer management.
 */

void test_transport_init() {
    pw_transport_t transport;
    pw_transport_init(&transport, -1); // use -1 to avoid actual fcntl

    assert(transport.fd == -1);
    assert(transport.read_pos == 0);
    assert(transport.read_len == 0);
    assert(transport.write_pos == 0);
    assert(transport.write_len == 0);

    printf("Transport init tests passed.\n");
}

int main() {
    test_transport_init();
    return 0;
}
