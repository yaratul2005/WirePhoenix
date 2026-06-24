#include "pw_connection.h"
#include <stdio.h>
#include <assert.h>

void test_valid_transitions() {
    pw_connection_t conn;
    pw_connection_init(&conn);
    assert(conn.state == PW_STATE_DISCONNECTED);

    assert(pw_connection_transition(&conn, PW_STATE_CONNECTING) == true);
    assert(conn.state == PW_STATE_CONNECTING);

    assert(pw_connection_transition(&conn, PW_STATE_HANDSHAKING) == true);
    assert(conn.state == PW_STATE_HANDSHAKING);

    assert(pw_connection_transition(&conn, PW_STATE_AUTHENTICATING) == true);
    assert(conn.state == PW_STATE_AUTHENTICATING);

    assert(pw_connection_transition(&conn, PW_STATE_READY) == true);
    assert(conn.state == PW_STATE_READY);

    assert(pw_connection_transition(&conn, PW_STATE_CLOSING) == true);
    assert(conn.state == PW_STATE_CLOSING);

    assert(pw_connection_transition(&conn, PW_STATE_CLOSED) == true);
    assert(conn.state == PW_STATE_CLOSED);

    printf("Valid transition tests passed.\n");
}

void test_invalid_transitions() {
    pw_connection_t conn;
    pw_connection_init(&conn);

    assert(pw_connection_transition(&conn, PW_STATE_READY) == false);
    assert(conn.state == PW_STATE_DISCONNECTED);

    assert(pw_connection_transition(&conn, PW_STATE_CONNECTING) == true);
    assert(pw_connection_transition(&conn, PW_STATE_READY) == false);

    printf("Invalid transition tests passed.\n");
}

int main() {
    test_valid_transitions();
    test_invalid_transitions();
    return 0;
}
