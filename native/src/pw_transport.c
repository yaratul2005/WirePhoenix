#include "pw_transport.h"
#include <string.h>

int pw_transport_set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

void pw_transport_init(pw_transport_t* transport, int fd) {
    if (transport) {
        transport->fd = fd;
        transport->read_pos = 0;
        transport->read_len = 0;
        transport->write_pos = 0;
        transport->write_len = 0;

        if (fd >= 0) {
            pw_transport_set_nonblocking(fd);
        }
    }
}

ssize_t pw_transport_read(pw_transport_t* transport) {
    if (!transport || transport->fd < 0) return -1;

    /* If buffer is full, we can't read more right now */
    if (transport->read_len == PW_BUFFER_SIZE) {
        return 0;
    }

    /* Compaction if needed to make space at the end */
    if (transport->read_len > 0 && transport->read_pos > 0) {
        memmove(transport->read_buffer,
                transport->read_buffer + transport->read_pos,
                transport->read_len);
        transport->read_pos = 0;
    } else if (transport->read_len == 0) {
        transport->read_pos = 0;
    }

    size_t space = PW_BUFFER_SIZE - transport->read_len;
    ssize_t n = recv(transport->fd,
                     transport->read_buffer + transport->read_pos + transport->read_len,
                     space, 0);

    if (n > 0) {
        transport->read_len += n;
    } else if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -EAGAIN;
        }
    }

    return n;
}

ssize_t pw_transport_write(pw_transport_t* transport) {
    if (!transport || transport->fd < 0) return -1;

    if (transport->write_len == 0) {
        return 0; /* Nothing to write */
    }

    ssize_t n = send(transport->fd,
                     transport->write_buffer + transport->write_pos,
                     transport->write_len, 0);

    if (n > 0) {
        transport->write_pos += n;
        transport->write_len -= n;

        if (transport->write_len == 0) {
            transport->write_pos = 0; /* Reset when empty */
        }
    } else if (n < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return -EAGAIN;
        }
    }

    return n;
}
