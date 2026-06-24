#ifndef PW_AUTH_H
#define PW_AUTH_H

#include "pw_frame.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PW_AUTH_METHOD_BEARER 1

typedef struct {
    uint8_t method;
    uint32_t token_len;
    const uint8_t* token;
} pw_auth_request_t;

typedef struct {
    uint8_t status; /* 0 = OK, non-zero = error */
    /* Could add optional error message or session details here */
} pw_auth_response_t;

/**
 * Serializes an auth request into a frame payload buffer.
 */
size_t pw_auth_serialize_request(const pw_auth_request_t* req, uint8_t* buffer, size_t buffer_len);

/**
 * Parses an auth request from a frame payload.
 */
size_t pw_auth_parse_request(const uint8_t* payload, size_t payload_len, pw_auth_request_t* out_req);

/**
 * Validates a bearer token. For the reference implementation, this checks against a fixed token.
 */
bool pw_auth_validate_bearer(const pw_auth_request_t* req, const char* expected_token);

#ifdef __cplusplus
}
#endif

#endif /* PW_AUTH_H */
