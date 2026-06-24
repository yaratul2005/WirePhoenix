#include "pw_varint.h"
#include "pw_frame.h"
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
    printf("Varint tests passed.\n");
}

void test_frame_serialize_parse() {
    pw_frame_t frame;
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
    assert(parsed_frame.version == PW_VERSION_1);

    printf("Frame tests passed.\n");
}

int main() {
    test_varint();
    test_frame_serialize_parse();
    return 0;
}
