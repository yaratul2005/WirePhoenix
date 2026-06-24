#include "pw_handshake.h"
#include <string.h>

/* Helper to write 32-bit uint in network byte order */
static void write_u32_be(uint8_t* buf, uint32_t val) {
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

/* Helper to read 32-bit uint in network byte order */
static uint32_t read_u32_be(const uint8_t* buf) {
    return ((uint32_t)buf[0] << 24) |
           ((uint32_t)buf[1] << 16) |
           ((uint32_t)buf[2] << 8)  |
           ((uint32_t)buf[3]);
}

size_t pw_handshake_serialize_client_hello(const pw_client_hello_t* hello, uint8_t* buffer, size_t buffer_len) {
    size_t required = 2 + 1 + 4 + PW_NONCE_SIZE;
    if (buffer_len < required) return 0;

    size_t offset = 0;
    buffer[offset++] = hello->magic[0];
    buffer[offset++] = hello->magic[1];
    buffer[offset++] = hello->version;

    write_u32_be(buffer + offset, hello->capabilities);
    offset += 4;

    memcpy(buffer + offset, hello->client_nonce, PW_NONCE_SIZE);
    offset += PW_NONCE_SIZE;

    return offset;
}

size_t pw_handshake_parse_client_hello(const uint8_t* buffer, size_t buffer_len, pw_client_hello_t* out_hello) {
    size_t required = 2 + 1 + 4 + PW_NONCE_SIZE;
    if (buffer_len < required) return 0;

    size_t offset = 0;
    out_hello->magic[0] = buffer[offset++];
    out_hello->magic[1] = buffer[offset++];
    out_hello->version = buffer[offset++];

    out_hello->capabilities = read_u32_be(buffer + offset);
    offset += 4;

    memcpy(out_hello->client_nonce, buffer + offset, PW_NONCE_SIZE);
    offset += PW_NONCE_SIZE;

    return offset;
}

size_t pw_handshake_serialize_server_hello(const pw_server_hello_t* hello, uint8_t* buffer, size_t buffer_len) {
    size_t required = 2 + 1 + 4 + PW_NONCE_SIZE + PW_SESSION_ID_SIZE + 1;
    if (buffer_len < required) return 0;

    size_t offset = 0;
    buffer[offset++] = hello->magic[0];
    buffer[offset++] = hello->magic[1];
    buffer[offset++] = hello->version;

    write_u32_be(buffer + offset, hello->capabilities);
    offset += 4;

    memcpy(buffer + offset, hello->server_nonce, PW_NONCE_SIZE);
    offset += PW_NONCE_SIZE;

    memcpy(buffer + offset, hello->session_id, PW_SESSION_ID_SIZE);
    offset += PW_SESSION_ID_SIZE;

    buffer[offset++] = hello->status;

    return offset;
}

size_t pw_handshake_parse_server_hello(const uint8_t* buffer, size_t buffer_len, pw_server_hello_t* out_hello) {
    size_t required = 2 + 1 + 4 + PW_NONCE_SIZE + PW_SESSION_ID_SIZE + 1;
    if (buffer_len < required) return 0;

    size_t offset = 0;
    out_hello->magic[0] = buffer[offset++];
    out_hello->magic[1] = buffer[offset++];
    out_hello->version = buffer[offset++];

    out_hello->capabilities = read_u32_be(buffer + offset);
    offset += 4;

    memcpy(out_hello->server_nonce, buffer + offset, PW_NONCE_SIZE);
    offset += PW_NONCE_SIZE;

    memcpy(out_hello->session_id, buffer + offset, PW_SESSION_ID_SIZE);
    offset += PW_SESSION_ID_SIZE;

    out_hello->status = buffer[offset++];

    return offset;
}
