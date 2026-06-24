#include "pw_auth.h"
#include <string.h>

static void write_u32_be(uint8_t* buf, uint32_t val) {
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

static uint32_t read_u32_be(const uint8_t* buf) {
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8)  |
           ((uint32_t)buf[3]);
}

size_t pw_auth_serialize_request(const pw_auth_request_t* req, uint8_t* buffer, size_t buffer_len) {
    size_t required = 1 + 4 + req->token_len;
    if (buffer_len < required) return 0;

    size_t offset = 0;
    buffer[offset++] = req->method;

    write_u32_be(buffer + offset, req->token_len);
    offset += 4;

    if (req->token_len > 0 && req->token != NULL) {
        memcpy(buffer + offset, req->token, req->token_len);
        offset += req->token_len;
    }

    return offset;
}

size_t pw_auth_parse_request(const uint8_t* payload, size_t payload_len, pw_auth_request_t* out_req) {
    if (payload_len < 5) return 0; /* min size: method(1) + len(4) */

    size_t offset = 0;
    out_req->method = payload[offset++];

    out_req->token_len = read_u32_be(payload + offset);
    offset += 4;

    if (payload_len - offset < out_req->token_len) return 0;

    out_req->token = payload + offset;
    offset += out_req->token_len;

    return offset;
}

bool pw_auth_validate_bearer(const pw_auth_request_t* req, const char* expected_token) {
    if (!req || req->method != PW_AUTH_METHOD_BEARER || !expected_token) return false;

    size_t expected_len = strlen(expected_token);
    if (req->token_len != expected_len) return false;

    return memcmp(req->token, expected_token, expected_len) == 0;
}
