#ifndef PW_TRANSPORT_H
#define PW_TRANSPORT_H

#include "pw_connection.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PW_BUFFER_SIZE 4096

typedef struct {
    int fd;

    /* Read buffer */
    uint8_t read_buffer[PW_BUFFER_SIZE];
    size_t read_pos;
    size_t read_len;

    /* Write buffer (simplified for now) */
    uint8_t write_buffer[PW_BUFFER_SIZE];
    size_t write_pos;
    size_t write_len;
} pw_transport_t;

/**
 * Sets a socket to non-blocking mode.
 */
int pw_transport_set_nonblocking(int fd);

/**
 * Initializes the transport for a connection.
 */
void pw_transport_init(pw_transport_t* transport, int fd);

/**
 * Reads from the socket into the read buffer.
 * Returns bytes read, or negative error code (e.g. -EAGAIN).
 */
ssize_t pw_transport_read(pw_transport_t* transport);

/**
 * Writes from the write buffer to the socket.
 * Returns bytes written, or negative error code (e.g. -EAGAIN).
 */
ssize_t pw_transport_write(pw_transport_t* transport);

#ifdef __cplusplus
}
#endif

#endif /* PW_TRANSPORT_H */
