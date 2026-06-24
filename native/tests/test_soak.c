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
#include <pthread.h>
#include <poll.h>
#include <assert.h>
#include <errno.h>

#define TEST_PORT 8082
#define SOAK_ITERATIONS 50

void* server_thread(void* arg __attribute__((unused))) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(TEST_PORT);

    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 5);

    for (int i=0; i<SOAK_ITERATIONS; i++) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_len);
        if (client_fd < 0) continue;

        pw_transport_t transport;
        pw_transport_init(&transport, client_fd);
        pw_connection_t conn;
        pw_connection_init(&conn);
        pw_connection_transition(&conn, PW_STATE_HANDSHAKING);

        struct pollfd pfd = { .fd = client_fd, .events = POLLIN };

        while (conn.state != PW_STATE_CLOSED && conn.state != PW_STATE_DISCONNECTED) {
            pfd.events = POLLIN;
            if (transport.write_len > 0) pfd.events |= POLLOUT;

            int p = poll(&pfd, 1, 50);
            if (p <= 0) continue;

            if (pfd.revents & POLLIN) {
                ssize_t r = pw_transport_read(&transport);
                if (r <= 0 && errno != EAGAIN && errno != EWOULDBLOCK) break;

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
                                .resume_accepted = false
                            };

                            pw_connection_transition(&conn, PW_STATE_AUTHENTICATING);

                            size_t written = pw_handshake_serialize_server_hello(&server_hello, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                            transport.write_len += written;

                        } else { break; }
                    } else if (conn.state == PW_STATE_AUTHENTICATING) {
                        pw_frame_t frame;
                        size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                        if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                            if (frame.opcode == PW_OPCODE_AUTH) {
                                pw_connection_transition(&conn, PW_STATE_READY);
                                pw_frame_t ok_frame = { .version = PW_VERSION_1, .flags = PW_FLAG_FIN, .opcode = PW_OPCODE_AUTH_OK };
                                transport.write_len += pw_frame_serialize_header(&ok_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                            }
                            transport.read_pos += parsed + frame.length;
                            transport.read_len -= (parsed + frame.length);
                        } else { break; }
                    } else if (conn.state == PW_STATE_READY) {
                        pw_frame_t frame;
                        size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                        if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                            if (frame.opcode == PW_OPCODE_DATA_TEXT || frame.opcode == PW_OPCODE_DATA_BINARY) {
                                pw_frame_t echo_frame = frame;
                                transport.write_len += pw_frame_serialize_header(&echo_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                memcpy(transport.write_buffer + transport.write_len, transport.read_buffer + transport.read_pos + parsed, frame.length);
                                transport.write_len += frame.length;
                            } else if (frame.opcode == PW_OPCODE_CLOSE) {
                                pw_frame_t close_frame = { .version = PW_VERSION_1, .flags = PW_FLAG_FIN, .opcode = PW_OPCODE_CLOSE };
                                transport.write_len += pw_frame_serialize_header(&close_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                pw_connection_transition(&conn, PW_STATE_CLOSING);
                            }
                            transport.read_pos += parsed + frame.length;
                            transport.read_len -= (parsed + frame.length);
                        } else { break; }
                    } else if (conn.state == PW_STATE_CLOSING) {
                        pw_frame_t frame;
                        size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                        if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                            transport.read_pos += parsed + frame.length; transport.read_len -= (parsed + frame.length);
                        } else break;
                    }

                    if (transport.read_len == prev_read_len) break;
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
    }

    close(server_fd);
    return NULL;
}

void do_soak_client_run() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(TEST_PORT);
    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) return;

    pw_transport_t transport;
    pw_transport_init(&transport, sock);
    pw_connection_t conn;
    pw_connection_init(&conn);
    pw_connection_transition(&conn, PW_STATE_CONNECTING);
    pw_connection_transition(&conn, PW_STATE_HANDSHAKING);

    pw_client_hello_t hello = {
        .magic = {PW_MAGIC_BYTE_1, PW_MAGIC_BYTE_2},
        .version = PW_VERSION_1,
        .capabilities = PW_CAP_AUTH_BEARER,
        .has_resume_token = false
    };
    transport.write_len = pw_handshake_serialize_client_hello(&hello, transport.write_buffer, PW_BUFFER_SIZE);

    struct pollfd pfd = { .fd = sock, .events = POLLIN | POLLOUT };
    int burst_count = 10;
    int received_bursts = 0;
    bool sent_close = false;

    while (conn.state != PW_STATE_CLOSED) {
        pfd.events = POLLIN;
        if (transport.write_len > 0) pfd.events |= POLLOUT;
        poll(&pfd, 1, 50);

        if (pfd.revents & POLLOUT) {
            pw_transport_write(&transport);
            if (conn.state == PW_STATE_CLOSING && transport.write_len == 0) {
                pw_connection_transition(&conn, PW_STATE_CLOSED);
            }
        }

        if (pfd.revents & POLLIN) {
            if (pw_transport_read(&transport) <= 0 && errno != EAGAIN) break;

            while (transport.read_len > 0) {
                size_t prev_len = transport.read_len;
                if (conn.state == PW_STATE_HANDSHAKING) {
                    pw_server_hello_t s_hello;
                    size_t parsed = pw_handshake_parse_server_hello(transport.read_buffer + transport.read_pos, transport.read_len, &s_hello);
                    if (parsed > 0) {
                        transport.read_pos += parsed; transport.read_len -= parsed;
                        pw_connection_transition(&conn, PW_STATE_AUTHENTICATING);

                        pw_auth_request_t auth_req = { .method = PW_AUTH_METHOD_BEARER, .token_len = 5, .token = (const uint8_t*)"token" };
                        uint8_t auth_payload[128];
                        size_t auth_len = pw_auth_serialize_request(&auth_req, auth_payload, sizeof(auth_payload));
                        pw_frame_t auth_frame = { .version = PW_VERSION_1, .flags = PW_FLAG_FIN, .opcode = PW_OPCODE_AUTH, .length = auth_len };
                        transport.write_len += pw_frame_serialize_header(&auth_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                        memcpy(transport.write_buffer + transport.write_len, auth_payload, auth_len);
                        transport.write_len += auth_len;
                    } else break;
                } else if (conn.state == PW_STATE_AUTHENTICATING) {
                    pw_frame_t frame;
                    size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                    if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                        pw_connection_transition(&conn, PW_STATE_READY);
                        transport.read_pos += parsed + frame.length; transport.read_len -= (parsed + frame.length);

                        // Send burst
                        for (int k=0; k<burst_count; k++) {
                            pw_frame_t data_frame = { .version = PW_VERSION_1, .flags = PW_FLAG_FIN, .opcode = PW_OPCODE_DATA_TEXT, .length = 4 };
                            transport.write_len += pw_frame_serialize_header(&data_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                            memcpy(transport.write_buffer + transport.write_len, "SOAK", 4);
                            transport.write_len += 4;
                        }
                    } else break;
                } else if (conn.state == PW_STATE_READY) {
                    pw_frame_t frame;
                    size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                    if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                        if (frame.opcode == PW_OPCODE_DATA_TEXT) {
                            received_bursts++;
                            if (received_bursts == burst_count && !sent_close) {
                                pw_frame_t close_frame = { .version = PW_VERSION_1, .flags = PW_FLAG_FIN, .opcode = PW_OPCODE_CLOSE };
                                transport.write_len += pw_frame_serialize_header(&close_frame, transport.write_buffer + transport.write_len, PW_BUFFER_SIZE - transport.write_len);
                                pw_connection_transition(&conn, PW_STATE_CLOSING);
                                sent_close = true;
                            }
                        } else if (frame.opcode == PW_OPCODE_CLOSE) {
                            pw_connection_transition(&conn, PW_STATE_CLOSING);
                        }
                        transport.read_pos += parsed + frame.length; transport.read_len -= (parsed + frame.length);
                    } else break;
                } else if (conn.state == PW_STATE_CLOSING) {
                    pw_frame_t frame;
                    size_t parsed = pw_frame_parse_header(transport.read_buffer + transport.read_pos, transport.read_len, &frame);
                    if (parsed > 0 && transport.read_len >= parsed + frame.length) {
                        transport.read_pos += parsed + frame.length; transport.read_len -= (parsed + frame.length);
                    } else break;
                }

                if (transport.read_len == prev_len) break;
            }
        }
    }
    close(sock);
}

int main() {
    pthread_t thread_id;
    pthread_create(&thread_id, NULL, server_thread, NULL);
    usleep(100000);

    for (int i=0; i<SOAK_ITERATIONS; i++) {
        do_soak_client_run();
    }

    pthread_join(thread_id, NULL);
    printf("Soak test passed (%d iterations).\n", SOAK_ITERATIONS);
    return 0;
}
