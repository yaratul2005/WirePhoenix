#include "pw_connection.h"
#include "pw_transport.h"
#include "pw_handshake.h"
#include "pw_auth.h"
#include "pw_frame.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <errno.h>

#define PORT 8080

int main() {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);
    fflush(stdout);

    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Very simple single connection echo server for testing
    while (1) {
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        printf("Client connected.\n");
        fflush(stdout);

        pw_transport_t transport;
        pw_transport_init(&transport, client_fd);
        pw_connection_t conn;
        pw_connection_init(&conn);
        pw_connection_transition(&conn, PW_STATE_CONNECTING);
        pw_connection_transition(&conn, PW_STATE_HANDSHAKING);

        struct pollfd pfd = { .fd = client_fd, .events = POLLIN };

        while (conn.state != PW_STATE_CLOSED && conn.state != PW_STATE_DISCONNECTED) {
            pfd.events = POLLIN;
            if (transport.write_len > 0) pfd.events |= POLLOUT;

            int p = poll(&pfd, 1, 100);
            if (p < 0) break;

            if (pfd.revents & POLLIN) {
                ssize_t r = pw_transport_read(&transport);
                if (r <= 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                    printf("Client disconnected (read %zd, errno %d).\n", r, errno);
                    fflush(stdout);
                    break;
                }

                while (transport.read_len > 0) {
                    size_t prev_read_len = transport.read_len;

                    if (conn.state == PW_STATE_HANDSHAKING) {
                        pw_client_hello_t client_hello;
                        size_t parsed = pw_handshake_parse_client_hello(transport.read_buffer + transport.read_pos, transport.read_len, &client_hello);

                        if (parsed > 0) {
                            transport.read_pos += parsed;
                            transport.read_len -= parsed;

                            pw_server_hello_t server_hello = {
                                .magic = {PW_MAGIC_BYTE_1, PW_MAGIC_BYTE_2},
                                .version = PW_VERSION_1,
                                .capabilities = client_hello.capabilities,
                                .status = 0,
                                .resume_accepted = client_hello.has_resume_token
                            };

                            size_t written = pw_handshake_serialize_server_hello(&server_hello, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                            transport.write_len += written;

                            pw_connection_transition(&conn, PW_STATE_AUTHENTICATING);
                            printf("Handshake complete.\n");
                            fflush(stdout);
                        } else {
                            break;
                        }
                    } else if (conn.state == PW_STATE_AUTHENTICATING) {
                        pw_frame_t frame;
                        size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                        if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                            if (frame.opcode == PW_OPCODE_AUTH) {
                                pw_auth_request_t req;
                                if (pw_auth_parse_request(transport.read_buffer + transport.read_pos + parsed, frame.length, &req)) {
                                    if (pw_auth_validate_bearer(&req, "secret_token")) {
                                        printf("Auth OK.\n");
                                        fflush(stdout);
                                        pw_connection_transition(&conn, PW_STATE_READY);

                                        pw_frame_t ok_frame = {
                                            .version = PW_VERSION_1,
                                            .flags = PW_FLAG_FIN,
                                            .opcode = PW_OPCODE_AUTH_OK,
                                            .stream_id = 0,
                                            .length = 0
                                        };

                                        size_t fw = pw_frame_serialize_header(&ok_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                        transport.write_len += fw;
                                    } else {
                                        printf("Auth Failed.\n");
                                        fflush(stdout);
                                        pw_connection_transition(&conn, PW_STATE_CLOSING);
                                    }
                                }
                            }

                            transport.read_pos += parsed + frame.length;
                            transport.read_len -= (parsed + frame.length);
                        } else {
                            break;
                        }
                    } else if (conn.state == PW_STATE_READY) {
                        pw_frame_t frame;
                        size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                        if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                            if (frame.opcode == PW_OPCODE_DATA_BINARY || frame.opcode == PW_OPCODE_DATA_TEXT) {
                                char buf[128] = {0};
                                memcpy(buf, transport.read_buffer + transport.read_pos + parsed, frame.length);
                                printf("Received data message (%u bytes): %s\n", frame.length, buf);
                                fflush(stdout);

                                pw_frame_t echo_frame = frame;
                                size_t head_len = pw_frame_serialize_header(&echo_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                transport.write_len += head_len;
                                memcpy(transport.write_buffer + transport.write_len, transport.read_buffer + transport.read_pos + parsed, frame.length);
                                transport.write_len += frame.length;
                            } else if (frame.opcode == PW_OPCODE_PING) {
                                printf("Received PING. Sending PONG...\n");
                                fflush(stdout);
                                pw_frame_t pong_frame = {
                                    .version = PW_VERSION_1,
                                    .flags = PW_FLAG_FIN,
                                    .opcode = PW_OPCODE_PONG,
                                    .stream_id = frame.stream_id,
                                    .length = 0
                                };
                                size_t fw = pw_frame_serialize_header(&pong_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                transport.write_len += fw;
                            } else if (frame.opcode == PW_OPCODE_CLOSE) {
                                printf("Received CLOSE.\n");
                                fflush(stdout);

                                pw_frame_t close_frame = {
                                    .version = PW_VERSION_1,
                                    .flags = PW_FLAG_FIN,
                                    .opcode = PW_OPCODE_CLOSE,
                                    .stream_id = 0,
                                    .length = 0
                                };
                                size_t hw = pw_frame_serialize_header(&close_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                transport.write_len += hw;

                                pw_connection_transition(&conn, PW_STATE_CLOSING);
                            }

                            transport.read_pos += parsed + frame.length;
                            transport.read_len -= (parsed + frame.length);
                        } else {
                            break;
                        }
                    } else {
                        break;
                    }

                    if (transport.read_len == prev_read_len) {
                        break;
                    }
                }
            }

            if (pfd.revents & POLLOUT) {
                pw_transport_write(&transport);
                if (conn.state == PW_STATE_CLOSING && transport.write_len == 0) {
                    pw_connection_transition(&conn, PW_STATE_CLOSED);
                }
            }
        }

        close(client_fd);
        printf("Connection closed.\n");
        fflush(stdout);
    }

    return 0;
}
