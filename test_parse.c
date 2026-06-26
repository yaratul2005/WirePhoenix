#include <stdio.h>
#include <stdint.h>
#include "pw_frame.h"
#include "pw_auth.h"

int main() {
    uint8_t buffer[] = {
        0x50, 0x57, 0x01, 0x80, 0x0F, // Frame Header
        0x00, // Stream ID
        0x11, // Length (17)
        0x01, // Auth Method
        0x00, 0x00, 0x00, 0x0C, // Token Len (12)
        's', 'e', 'c', 'r', 'e', 't', '_', 't', 'o', 'k', 'e', 'n'
    };

    pw_frame_t frame;
    size_t parsed = pw_frame_parse_header(buffer, sizeof(buffer), &frame);
    printf("parsed header: %zu\n", parsed);
    if (parsed > 0) {
        printf("opcode: %d, length: %u\n", frame.opcode, frame.length);
        pw_auth_request_t req;
        size_t auth_parsed = pw_auth_parse_request(buffer + parsed, frame.length, &req);
        printf("auth_parsed: %zu\n", auth_parsed);
        if (auth_parsed > 0) {
            printf("token_len: %u\n", req.token_len);
        }
    }

    return 0;
}
