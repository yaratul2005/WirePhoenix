#ifndef PW_VARINT_H
#define PW_VARINT_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encodes an unsigned 32-bit integer as a base-128 varint.
 * @param value The value to encode.
 * @param buffer The buffer to write to (must be at least 5 bytes).
 * @return The number of bytes written.
 */
size_t pw_varint_encode32(uint32_t value, uint8_t* buffer);

/**
 * Decodes a base-128 varint to an unsigned 32-bit integer.
 * @param buffer The buffer to read from.
 * @param max_len The maximum number of bytes to read.
 * @param out_value Pointer to store the decoded value.
 * @return The number of bytes read, or 0 on error (e.g. truncated or too large).
 */
size_t pw_varint_decode32(const uint8_t* buffer, size_t max_len, uint32_t* out_value);

#ifdef __cplusplus
}
#endif

#endif /* PW_VARINT_H */
