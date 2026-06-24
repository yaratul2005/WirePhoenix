#ifndef PW_HANDSHAKE_H
#define PW_HANDSHAKE_H

#include "phoenixwire.h"
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Capability Bits */
#define PW_CAP_COMPRESSION    0x01
#define PW_CAP_MULTIPLEXING   0x02
#define PW_CAP_AUTH_BEARER    0x04
#define PW_CAP_AUTH_HMAC      0x08

#define PW_NONCE_SIZE 16
#define PW_SESSION_ID_SIZE 16

typedef struct {
    uint8_t magic[2];
    uint8_t version;
    uint32_t capabilities;
    uint8_t client_nonce[PW_NONCE_SIZE];
    /* For simplicity in this iteration, optional auth tokens are omitted or handled out-of-band/via separate frame */
} pw_client_hello_t;

typedef struct {
    uint8_t magic[2];
    uint8_t version;
    uint32_t capabilities;
    uint8_t server_nonce[PW_NONCE_SIZE];
    uint8_t session_id[PW_SESSION_ID_SIZE];
    uint8_t status; /* 0 = OK, non-zero = error code */
} pw_server_hello_t;

/**
 * Serializes a client hello into a buffer.
 */
size_t pw_handshake_serialize_client_hello(const pw_client_hello_t* hello, uint8_t* buffer, size_t buffer_len);

/**
 * Parses a client hello from a buffer.
 */
size_t pw_handshake_parse_client_hello(const uint8_t* buffer, size_t buffer_len, pw_client_hello_t* out_hello);

/**
 * Serializes a server hello into a buffer.
 */
size_t pw_handshake_serialize_server_hello(const pw_server_hello_t* hello, uint8_t* buffer, size_t buffer_len);

/**
 * Parses a server hello from a buffer.
 */
size_t pw_handshake_parse_server_hello(const uint8_t* buffer, size_t buffer_len, pw_server_hello_t* out_hello);

#ifdef __cplusplus
}
#endif

#endif /* PW_HANDSHAKE_H */
