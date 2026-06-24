#include "pw_varint.h"

size_t pw_varint_encode32(uint32_t value, uint8_t* buffer) {
    size_t i = 0;
    while (value >= 0x80) {
        buffer[i++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    buffer[i++] = (uint8_t)(value);
    return i;
}

size_t pw_varint_decode32(const uint8_t* buffer, size_t max_len, uint32_t* out_value) {
    uint32_t result = 0;
    size_t shift = 0;
    size_t i = 0;

    while (i < max_len && i < 5) { /* max 5 bytes for 32-bit */
        uint8_t byte = buffer[i++];
        result |= (uint32_t)(byte & 0x7F) << shift;
        if ((byte & 0x80) == 0) {
            /* If this is the 5th byte, the top 4 bits must be 0 to fit in 32 bits.
               Wait, 4 bytes * 7 bits = 28 bits. The 5th byte provides 4 bits.
               If byte & 0xF0 != 0, it overflows a 32-bit uint. */
            if (i == 5 && (byte & 0xF0) != 0) {
                return 0; /* Overflow error */
            }
            *out_value = result;
            return i;
        }
        shift += 7;
    }

    /* Reached end of max_len without finding terminal byte, OR exceeded 5 bytes without terminating. */
    return 0;
}
