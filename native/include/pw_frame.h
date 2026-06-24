#ifndef PW_FRAME_H
#define PW_FRAME_H

#include "phoenixwire.h"
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Calculates the required buffer size to serialize the given frame header.
 */
size_t pw_frame_header_size(const pw_frame_t* frame);

/**
 * Serializes a frame header into the provided buffer.
 * @param frame The frame to serialize.
 * @param buffer The buffer to write to.
 * @param buffer_len The length of the buffer.
 * @return The number of bytes written, or 0 on error.
 */
size_t pw_frame_serialize_header(const pw_frame_t* frame, uint8_t* buffer, size_t buffer_len);

/**
 * Parses a frame header from the given buffer.
 * @param buffer The buffer containing the raw frame.
 * @param buffer_len The number of bytes available.
 * @param out_frame The frame structure to populate.
 * @return The number of bytes consumed by the header, or 0 if incomplete/invalid.
 */
size_t pw_frame_parse_header(const uint8_t* buffer, size_t buffer_len, pw_frame_t* out_frame);

#ifdef __cplusplus
}
#endif

#endif /* PW_FRAME_H */
