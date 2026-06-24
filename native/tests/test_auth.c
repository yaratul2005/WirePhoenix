#include "pw_auth.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_auth_bearer() {
    const char* my_token = "super_secret_token_123";
    pw_auth_request_t req = {
        .method = PW_AUTH_METHOD_BEARER,
        .token_len = strlen(my_token),
        .token = (const uint8_t*)my_token
    };

    uint8_t buffer[128];
    size_t written = pw_auth_serialize_request(&req, buffer, sizeof(buffer));
    assert(written > 0);

    pw_auth_request_t parsed;
    size_t read = pw_auth_parse_request(buffer, written, &parsed);
    assert(read == written);

    assert(parsed.method == PW_AUTH_METHOD_BEARER);
    assert(parsed.token_len == strlen(my_token));
    assert(memcmp(parsed.token, my_token, parsed.token_len) == 0);

    assert(pw_auth_validate_bearer(&parsed, "super_secret_token_123") == true);
    assert(pw_auth_validate_bearer(&parsed, "wrong_token") == false);

    printf("Auth tests passed.\n");
}

int main() {
    test_auth_bearer();
    return 0;
}
