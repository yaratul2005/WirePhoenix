#ifndef PHOENIXWIRE_H
#define PHOENIXWIRE_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * PhoenixWire Transport Protocol Definitions
 */

/* Magic Bytes & Version */
#define PW_MAGIC_BYTE_1 0x50 /* 'P' */
#define PW_MAGIC_BYTE_2 0x57 /* 'W' */
#define PW_VERSION_1    0x01

/* Connection States */
typedef enum {
    PW_STATE_DISCONNECTED = 0,
    PW_STATE_CONNECTING = 1,
    PW_STATE_HANDSHAKING = 2,
    PW_STATE_AUTHENTICATING = 3,
    PW_STATE_READY = 4,
    PW_STATE_DRAINING = 5,
    PW_STATE_CLOSING = 6,
    PW_STATE_CLOSED = 7
} pw_connection_state_t;

/* Opcodes */
typedef enum {
    PW_OPCODE_DATA_BINARY = 0x01,
    PW_OPCODE_DATA_TEXT   = 0x02,
    PW_OPCODE_PING        = 0x03,
    PW_OPCODE_PONG        = 0x04,
    PW_OPCODE_CLOSE       = 0x05,
    PW_OPCODE_AUTH        = 0x06,
    PW_OPCODE_AUTH_OK     = 0x07,
    PW_OPCODE_AUTH_FAIL   = 0x08,
    PW_OPCODE_ACK         = 0x09,
    PW_OPCODE_NACK        = 0x0A,
    PW_OPCODE_CONTROL     = 0x0B
} pw_opcode_t;

/* Flags */
#define PW_FLAG_COMPRESSED 0x80
#define PW_FLAG_ACK_REQ    0x40
#define PW_FLAG_PRIORITY   0x20
#define PW_FLAG_FIN        0x10
#define PW_FLAG_RESERVED_3 0x08
#define PW_FLAG_RESERVED_2 0x04
#define PW_FLAG_RESERVED_1 0x02
#define PW_FLAG_RESERVED_0 0x01

/* Error Codes */
#define PW_ERR_NORMAL_CLOSE        1000
#define PW_ERR_GOING_AWAY          1001
#define PW_ERR_PROTOCOL_ERROR      1002
#define PW_ERR_UNSUPPORTED_DATA    1003
#define PW_ERR_AUTH_FAILED         1004
#define PW_ERR_TIMEOUT             1005
#define PW_ERR_CONNECTION_DROPPED  1006
#define PW_ERR_PAYLOAD_TOO_LARGE   1007
#define PW_ERR_RATE_LIMITED        1008
#define PW_ERR_INTERNAL_ERROR      1011

/* Delivery Modes */
typedef enum {
    PW_DELIVERY_BEST_EFFORT = 0,
    PW_DELIVERY_AT_LEAST_ONCE = 1,
    PW_DELIVERY_EXACTLY_ONCE = 2
} pw_delivery_mode_t;

/*
 * Note: Frame headers in memory might differ from on-wire format due to
 * varint encoding of stream_id and length.
 * The parser will populate a structure similar to this.
 */
typedef struct {
    uint8_t magic[2];
    uint8_t version;
    uint8_t flags;
    uint8_t opcode;
    uint32_t stream_id; /* Expanded from varint */
    uint32_t length;    /* Expanded from varint */
    uint32_t header_crc;
    const uint8_t* payload;
} pw_frame_t;


#ifdef __cplusplus
}
#endif

#endif /* PHOENIXWIRE_H */
