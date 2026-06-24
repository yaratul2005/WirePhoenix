#include "pw_connection.h"
#include "pw_transport.h"
#include "pw_handshake.h"
#include "pw_auth.h"
#include "pw_frame.h"
#include "pw_heartbeat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <poll.h>
#include <errno.h>

#define PORT 8080

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    printf("Connected to server.\n");
    fflush(stdout);

    pw_transport_t transport;
    pw_transport_init(&transport, sock);
    pw_connection_t conn;
    pw_connection_init(&conn);
    pw_connection_transition(&conn, PW_STATE_CONNECTING);
    pw_connection_transition(&conn, PW_STATE_HANDSHAKING);

    // Send Handshake
    pw_client_hello_t hello = {
        .magic = {PW_MAGIC_BYTE_1, PW_MAGIC_BYTE_2},
        .version = PW_VERSION_1,
        .capabilities = PW_CAP_AUTH_BEARER,
        .has_resume_token = false
    };

    transport.write_len = pw_handshake_serialize_client_hello(&hello, transport.write_buffer, PW_BUFFER_SIZE);

    struct pollfd pfd = { .fd = sock, .events = POLLIN | POLLOUT };

    while (conn.state != PW_STATE_CLOSED && conn.state != PW_STATE_DISCONNECTED) {
        pfd.events = POLLIN;
        if (transport.write_len > 0) pfd.events |= POLLOUT;

        int p = poll(&pfd, 1, 100);
        if (p < 0) break;

        if (pfd.revents & POLLOUT) {
            pw_transport_write(&transport);
            if (conn.state == PW_STATE_CLOSING && transport.write_len == 0) {
                pw_connection_transition(&conn, PW_STATE_CLOSED);
            }
        }

        if (pfd.revents & POLLIN) {
            ssize_t r = pw_transport_read(&transport);
            if (r <= 0 && errno != EAGAIN && errno != EWOULDBLOCK) {
                printf("Server disconnected (read %zd, errno %d).\n", r, errno);
                fflush(stdout);
                break;
            }

            while (transport.read_len > 0) {
                size_t prev_read_len = transport.read_len;

                if (conn.state == PW_STATE_HANDSHAKING) {
                    pw_server_hello_t s_hello;
                    size_t parsed = pw_handshake_parse_server_hello(transport.read_buffer + transport.read_pos, transport.read_len, &s_hello);
                    if (parsed > 0) {
                        transport.read_pos += parsed;
                        transport.read_len -= parsed;
                        printf("Handshake accepted.\n");
                        fflush(stdout);
                        pw_connection_transition(&conn, PW_STATE_AUTHENTICATING);

                        // Send Auth
                        pw_auth_request_t auth_req = {
                            .method = PW_AUTH_METHOD_BEARER,
                            .token_len = strlen("secret_token"),
                            .token = (const uint8_t*)"secret_token"
                        };

                        uint8_t auth_payload[128];
                        size_t auth_len = pw_auth_serialize_request(&auth_req, auth_payload, sizeof(auth_payload));

                        pw_frame_t auth_frame = {
                            .version = PW_VERSION_1,
                            .flags = PW_FLAG_FIN,
                            .opcode = PW_OPCODE_AUTH,
                            .stream_id = 0,
                            .length = auth_len
                        };

                        size_t hw = pw_frame_serialize_header(&auth_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                        transport.write_len += hw;
                        memcpy(transport.write_buffer + transport.write_len, auth_payload, auth_len);
                        transport.write_len += auth_len;
                    } else {
                        break;
                    }
                } else if (conn.state == PW_STATE_AUTHENTICATING) {
                    pw_frame_t frame;
                    size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                    if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                        if (frame.opcode == PW_OPCODE_AUTH_OK) {
                            printf("Auth OK.\n");
                            fflush(stdout);
                            pw_connection_transition(&conn, PW_STATE_READY);

                            // Send Data
                            const char* msg = "Hello from client!";
                            pw_frame_t data_frame = {
                                .version = PW_VERSION_1,
                                .flags = PW_FLAG_FIN,
                                .opcode = PW_OPCODE_DATA_TEXT,
                                .stream_id = 1,
                                .length = strlen(msg)
                            };
                            size_t hw = pw_frame_serialize_header(&data_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                            transport.write_len += hw;
                            memcpy(transport.write_buffer + transport.write_len, msg, strlen(msg));
                            transport.write_len += strlen(msg);
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
                        if (frame.opcode == PW_OPCODE_DATA_TEXT) {
                            char buf[128] = {0};
                            memcpy(buf, transport.read_buffer + transport.read_pos + parsed, frame.length);
                            printf("Received Echo: %s\n", buf);
                            fflush(stdout);

                            // Send PING
                            pw_frame_t ping_frame = {
                                .version = PW_VERSION_1,
                                .flags = PW_FLAG_FIN,
                                .opcode = PW_OPCODE_PING,
                                .stream_id = 0,
                                .length = 0
                            };
                            size_t hw = pw_frame_serialize_header(&ping_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                            transport.write_len += hw;
                        } else if (frame.opcode == PW_OPCODE_PONG) {
                            printf("Received PONG. Initiating graceful CLOSE.\n");
                            fflush(stdout);

                            // Send CLOSE
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
                        } else if (frame.opcode == PW_OPCODE_CLOSE) {
                            printf("Server closed connection.\n");
                            fflush(stdout);
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
    }

    close(sock);
    printf("Client exited.\n");
    fflush(stdout);

    return 0;
}
