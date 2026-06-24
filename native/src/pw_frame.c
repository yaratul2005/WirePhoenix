#include "pw_frame.h"
#include "pw_varint.h"
#include <string.h>

size_t pw_frame_header_size(const pw_frame_t* frame) {
    size_t size = 4; /* Magic(2) + Flags(1) + Opcode(1) */

    /* Calculate varint sizes (dummy encoding) */
    uint8_t dummy[5];
    size += pw_varint_encode32(frame->stream_id, dummy);
    size += pw_varint_encode32(frame->length, dummy);

    /* Optional CRC (if we implement it, typically 4 bytes) */
    /* size += 4; */

    return size;
}

size_t pw_frame_serialize_header(const pw_frame_t* frame, uint8_t* buffer, size_t buffer_len) {
    if (buffer_len < pw_frame_header_size(frame)) {
        return 0;
    }

    size_t offset = 0;

    /* Magic & Version handled together? The spec says Magic/version is 2 bytes,
       but we have PW_MAGIC_BYTE_1, PW_MAGIC_BYTE_2, and PW_VERSION_1.
       Let's assume:
       Byte 0: Magic 1 (0x50)
       Byte 1: Version (0x01)
       (If magic was 2 bytes and version 1 byte, that's 3 bytes).
       Let's pack it as: Byte 0: Magic, Byte 1: Version for now to match 2 bytes.
       Wait, let's use 'P' (0x50) and Version (0x01) for the 2 bytes.
    */
    buffer[offset++] = PW_MAGIC_BYTE_1;
    buffer[offset++] = PW_VERSION_1;

    buffer[offset++] = frame->flags;
    buffer[offset++] = frame->opcode;

    offset += pw_varint_encode32(frame->stream_id, buffer + offset);
    offset += pw_varint_encode32(frame->length, buffer + offset);

    /* CRC serialization would go here */

    return offset;
}

size_t pw_frame_parse_header(const uint8_t* buffer, size_t buffer_len, pw_frame_t* out_frame) {
    if (buffer_len < 4) { /* Minimum possible size */
        return 0;
    }

    size_t offset = 0;

    out_frame->magic[0] = buffer[offset++];
    out_frame->version = buffer[offset++];
    out_frame->flags = buffer[offset++];
    out_frame->opcode = buffer[offset++];

    size_t read_bytes = pw_varint_decode32(buffer + offset, buffer_len - offset, &out_frame->stream_id);
    if (read_bytes == 0) return 0;
    offset += read_bytes;

    read_bytes = pw_varint_decode32(buffer + offset, buffer_len - offset, &out_frame->length);
    if (read_bytes == 0) return 0;
    offset += read_bytes;

    /* CRC parsing would go here */

    out_frame->payload = NULL; /* Not parsed here */

    return offset;
}
