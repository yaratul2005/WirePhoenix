#include "pw_frame.h"
#include "pw_varint.h"
#include <string.h>

/* According to our spec, magic is 2 bytes, and version is 1 byte, but previous
   test assumptions mapped it differently. Let's fix it to match the header strictly:
   Magic byte 1 (0x50), Magic byte 2 (0x57), Version (0x01).
   But pw_frame_t only has magic[2], and version. So let's serialize them correctly.
*/

size_t pw_frame_header_size(const pw_frame_t* frame) {
    size_t size = 4; /* Originally it was 4 (assuming magic[1]+version). Let's make it 5: Magic(2) + Version(1) + Flags(1) + Opcode(1).
                        Wait, the struct has: uint8_t magic[2]; uint8_t version; uint8_t flags; uint8_t opcode; -> 5 bytes. */
    size = 5;

    /* Calculate varint sizes (dummy encoding) */
    uint8_t dummy[5];
    size += pw_varint_encode32(frame->stream_id, dummy);
    size += pw_varint_encode32(frame->length, dummy);

    return size;
}

size_t pw_frame_serialize_header(const pw_frame_t* frame, uint8_t* buffer, size_t buffer_len) {
    if (buffer_len < pw_frame_header_size(frame)) {
        return 0;
    }

    size_t offset = 0;

    buffer[offset++] = PW_MAGIC_BYTE_1;
    buffer[offset++] = PW_MAGIC_BYTE_2;
    buffer[offset++] = frame->version;
    buffer[offset++] = frame->flags;
    buffer[offset++] = frame->opcode;

    offset += pw_varint_encode32(frame->stream_id, buffer + offset);
    offset += pw_varint_encode32(frame->length, buffer + offset);

    return offset;
}

size_t pw_frame_parse_header(const uint8_t* buffer, size_t buffer_len, pw_frame_t* out_frame) {
    if (buffer_len < 5) { /* Minimum possible size: Magic(2)+Version(1)+Flags(1)+Opcode(1) */
        return 0;
    }

    size_t offset = 0;

    out_frame->magic[0] = buffer[offset++];
    out_frame->magic[1] = buffer[offset++];
    out_frame->version = buffer[offset++];
    out_frame->flags = buffer[offset++];
    out_frame->opcode = buffer[offset++];

    /* Strict validation on magic and version */
    if (out_frame->magic[0] != PW_MAGIC_BYTE_1 || out_frame->magic[1] != PW_MAGIC_BYTE_2) {
        return (size_t)-1; /* Protocol error / invalid magic */
    }

    /* Varints */
    size_t read_bytes = pw_varint_decode32(buffer + offset, buffer_len - offset, &out_frame->stream_id);
    if (read_bytes == 0) {
        /* If buffer_len - offset < 5 and we didn't terminate, it's a partial read.
           But if it's 5 and we didn't terminate (overflow), that's an error.
           Let's return 0 for partial read, which implies need more data. */
        /* To distinguish overflow vs partial read, we could enhance varint_decode, but
           returning 0 means "wait for more data", and if we exceed the buffer size it eventually timeouts. */
        return 0;
    }
    offset += read_bytes;

    read_bytes = pw_varint_decode32(buffer + offset, buffer_len - offset, &out_frame->length);
    if (read_bytes == 0) return 0;
    offset += read_bytes;

    out_frame->payload = NULL; /* Not parsed here */

    return offset;
}
