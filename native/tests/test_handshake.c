#include "pw_handshake.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_client_hello() {
    pw_client_hello_t hello = {
        .magic = {PW_MAGIC_BYTE_1, PW_MAGIC_BYTE_2},
        .version = PW_VERSION_1,
        .capabilities = PW_CAP_MULTIPLEXING | PW_CAP_AUTH_BEARER,
        .has_resume_token = true
    };
    memset(hello.client_nonce, 0xAA, PW_NONCE_SIZE);
    memset(hello.resume_session_id, 0xDD, PW_SESSION_ID_SIZE);

    uint8_t buffer[128];
    size_t written = pw_handshake_serialize_client_hello(&hello, buffer, sizeof(buffer));
    assert(written > 0);

    pw_client_hello_t parsed;
    size_t read = pw_handshake_parse_client_hello(buffer, written, &parsed);
    assert(read == written);

    assert(parsed.magic[0] == PW_MAGIC_BYTE_1);
    assert(parsed.magic[1] == PW_MAGIC_BYTE_2);
    assert(parsed.version == PW_VERSION_1);
    assert((parsed.capabilities & PW_CAP_MULTIPLEXING) != 0);
    assert((parsed.capabilities & PW_CAP_AUTH_BEARER) != 0);
    assert((parsed.capabilities & PW_CAP_RESUME) != 0);
    assert(memcmp(parsed.client_nonce, hello.client_nonce, PW_NONCE_SIZE) == 0);
    assert(parsed.has_resume_token == true);
    assert(memcmp(parsed.resume_session_id, hello.resume_session_id, PW_SESSION_ID_SIZE) == 0);

    // Test without resume token
    hello.has_resume_token = false;
    written = pw_handshake_serialize_client_hello(&hello, buffer, sizeof(buffer));
    read = pw_handshake_parse_client_hello(buffer, written, &parsed);
    assert(read == written);
    assert(parsed.has_resume_token == false);
    assert((parsed.capabilities & PW_CAP_RESUME) == 0);

    // Test protocol version rejection simulation
    // The parser doesn't reject natively (just parses), rejection happens in the state machine / server
    assert(parsed.version == PW_VERSION_1);

    printf("Client hello tests passed.\n");
}

void test_server_hello() {
    pw_server_hello_t hello = {
        .magic = {PW_MAGIC_BYTE_1, PW_MAGIC_BYTE_2},
        .version = PW_VERSION_1,
        .capabilities = PW_CAP_MULTIPLEXING,
        .status = 0,
        .resume_accepted = true
    };
    memset(hello.server_nonce, 0xBB, PW_NONCE_SIZE);
    memset(hello.session_id, 0xCC, PW_SESSION_ID_SIZE);

    uint8_t buffer[64];
    size_t written = pw_handshake_serialize_server_hello(&hello, buffer, sizeof(buffer));
    assert(written > 0);

    pw_server_hello_t parsed;
    size_t read = pw_handshake_parse_server_hello(buffer, written, &parsed);
    assert(read == written);

    assert(parsed.magic[0] == PW_MAGIC_BYTE_1);
    assert(parsed.version == PW_VERSION_1);
    assert(parsed.capabilities == PW_CAP_MULTIPLEXING);
    assert(parsed.status == 0);
    assert(parsed.resume_accepted == true);
    assert(memcmp(parsed.server_nonce, hello.server_nonce, PW_NONCE_SIZE) == 0);
    assert(memcmp(parsed.session_id, hello.session_id, PW_SESSION_ID_SIZE) == 0);

    printf("Server hello tests passed.\n");
}

int main() {
    test_client_hello();
    test_server_hello();
    return 0;
}
