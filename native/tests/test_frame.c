#include "pw_frame.h"
#include "pw_varint.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

void test_varint() {
    uint8_t buffer[10];
    size_t written = pw_varint_encode32(300, buffer);
    assert(written > 0);

    uint32_t decoded;
    size_t read = pw_varint_decode32(buffer, written, &decoded);
    assert(read == written);
    assert(decoded == 300);

    // Overflow test (5 bytes, but msb of 5th byte is 1)
    uint8_t overflow_buf[5] = {0xFF, 0xFF, 0xFF, 0xFF, 0x10}; // 0x10 is 00010000, > 0x0F
    assert(pw_varint_decode32(overflow_buf, 5, &decoded) == 0);

    printf("Varint tests passed.\n");
}

void test_frame_serialize_parse() {
    pw_frame_t frame;
    frame.version = PW_VERSION_1;
    frame.flags = PW_FLAG_FIN;
    frame.opcode = PW_OPCODE_DATA_BINARY;
    frame.stream_id = 42;
    frame.length = 1024;

    uint8_t buffer[64];
    size_t written = pw_frame_serialize_header(&frame, buffer, sizeof(buffer));
    assert(written > 0);

    pw_frame_t parsed_frame;
    size_t read = pw_frame_parse_header(buffer, written, &parsed_frame);
    assert(read == written);
    assert(parsed_frame.flags == PW_FLAG_FIN);
    assert(parsed_frame.opcode == PW_OPCODE_DATA_BINARY);
    assert(parsed_frame.stream_id == 42);
    assert(parsed_frame.length == 1024);
    assert(parsed_frame.magic[0] == PW_MAGIC_BYTE_1);
    assert(parsed_frame.magic[1] == PW_MAGIC_BYTE_2);
    assert(parsed_frame.version == PW_VERSION_1);

    // Partial read test
    assert(pw_frame_parse_header(buffer, written - 1, &parsed_frame) == 0);

    // Invalid magic test
    buffer[0] = 0x00;
    assert(pw_frame_parse_header(buffer, written, &parsed_frame) == (size_t)-1);

    printf("Frame tests passed.\n");
}

int main() {
    test_varint();
    test_frame_serialize_parse();
    return 0;
}
