# Technical Specification: PhoenixWire Transport

## 1. Purpose

PhoenixWire is a custom real-time transport protocol for PHP applications that need lower overhead than WebSocket and do not require browser interoperability. The system is designed for server-to-server, mobile, IoT, gaming, and internal application traffic, where compact framing, predictable latency, and native integration matter more than browser compatibility. [openswoole](https://openswoole.com/docs)

## 2. Goals

### Primary goals
- Minimize connection setup cost.
- Minimize per-message bandwidth overhead.
- Minimize CPU usage for framing, parsing, and transport.
- Provide a PHP-friendly API with production-grade performance.
- Support secure, bidirectional, message-oriented communication.

### Non-goals
- Browser compatibility.
- Drop-in replacement for RFC 6455 clients.
- Human-readable wire format by default.
- Pure PHP implementation of the transport core.

## 3. System Overview

PhoenixWire consists of five layers:
1. Native transport core in C or C++.
2. Protocol codec and session manager.
3. Security and authentication layer.
4. PHP extension API.
5. Optional standalone daemon and client tools.

The performance-critical network path must live entirely in native code. PHP FFI can be used for early prototypes, but the PHP manual notes that FFI is not meant to replace native extensions and that FFI access is slower than native PHP array/object access. Open Swoole is a useful reference for coroutine-based non-blocking networking patterns in PHP, but PhoenixWire should not depend on it for core transport performance. [php.dev.org](https://php.dev.org.tw/manual/en/intro.ffi.php)

## 4. Architecture

### 4.1 Native Core
The native core is responsible for:
- TCP socket handling.
- TLS termination.
- Frame encoding/decoding.
- Connection state management.
- Backpressure and send queue management.
- Heartbeats and timeout handling.
- Authentication negotiation.
- Event dispatch.

### 4.2 PHP Extension
The PHP extension provides:
- OOP connection objects.
- Async callback hooks or polling APIs.
- Session metadata access.
- Send/receive methods.
- Error handling and stats reporting.

### 4.3 Optional Daemon
A standalone daemon may be shipped for:
- Reference server implementation.
- Load testing.
- Cross-language interoperability.
- Debugging and protocol inspection.

## 5. Protocol Design

### 5.1 Transport
- Primary transport: TCP.
- Security transport: TLS 1.3 over TCP.
- Optional future transport: QUIC/UDP variant, only if justified later.

### 5.2 Connection Lifecycle
A PhoenixWire connection has these states:
- `DISCONNECTED`
- `CONNECTING`
- `HANDSHAKING`
- `AUTHENTICATING`
- `READY`
- `DRAINING`
- `CLOSING`
- `CLOSED`

### 5.3 Handshake
The handshake is a compact binary exchange, not HTTP Upgrade. It should include:
- Magic bytes.
- Protocol version.
- Capability bitmask.
- Client nonce.
- Optional auth token.
- Server response nonce.
- Session identifier.
- Negotiated features.

### 5.4 Framing
Each frame contains:
- Frame header.
- Message type / opcode.
- Flags.
- Stream or channel ID.
- Payload length.
- Optional integrity field.
- Payload.

### 5.5 Suggested Header Layout
A recommended base frame header:

| Field | Size | Description |
|---|---:|---|
| Magic/version | 2 bytes | Protocol identification |
| Flags | 1 byte | Compression, ack, priority, fin, reserved |
| Opcode | 1 byte | Data, ping, pong, close, auth, control |
| Stream ID | varint | Logical channel identifier |
| Length | varint | Payload size |
| Header CRC | optional | Integrity for header |
| Payload | variable | Message body |

## 6. Message Model

PhoenixWire must support:
- Binary messages.
- Text messages as an application-level option.
- Control frames.
- Close frames.
- Ping/pong heartbeat frames.
- Optional fragmented messages.
- Optional multiplexed logical streams.

### 6.1 Message Types
- `DATA_BINARY`
- `DATA_TEXT`
- `PING`
- `PONG`
- `CLOSE`
- `AUTH`
- `AUTH_OK`
- `AUTH_FAIL`
- `ACK`
- `NACK`
- `CONTROL`

## 7. Flow Control

### 7.1 Backpressure
The server and client must both maintain:
- Per-connection outbound queue limits.
- Max frame size limits.
- Max in-flight messages.
- Optional credit-based flow control for streams.

### 7.2 Delivery Modes
Support at least:
- Best effort.
- At-least-once with application ACKs.
- Exactly-once only if the application layer implements idempotency and deduplication.

## 8. Multiplexing

PhoenixWire should support logical channels over one physical connection. Each stream has:
- Stream ID.
- Optional priority.
- Independent ordering guarantees.
- Independent cancellation and close semantics.

This is important for avoiding head-of-line blocking across unrelated traffic types.

## 9. Security

### 9.1 Transport Security
- TLS 1.3 required by default.
- Plain TCP allowed only for local development or trusted internal lab use.

### 9.2 Authentication
Support one or more auth methods:
- Bearer token.
- HMAC challenge-response.
- Signed session ticket.
- Mutual TLS for enterprise deployments.

### 9.3 Integrity
Add optional per-frame integrity:
- CRC32 for debugging and corruption detection.
- HMAC or AEAD tag for authenticated frames if needed.

### 9.4 Abuse Prevention
Implement:
- Handshake rate limiting.
- Connection quotas.
- Token expiration.
- Replay protection.
- Frame size caps.
- Invalid opcode rejection.

## 10. PHP API Specification

### 10.1 Core Classes
- `PhoenixWire\Client`
- `PhoenixWire\Server`
- `PhoenixWire\Session`
- `PhoenixWire\Message`
- `PhoenixWire\Stats`

### 10.2 Client Methods
- `connect(string $host, int $port, array $options = []): Client`
- `send(string|Message $data, array $options = []): void`
- `receive(int $timeoutMs = 0): ?Message`
- `close(int $code = 1000, string $reason = ''): void`
- `ping(string $payload = ''): void`

### 10.3 Server Methods
- `listen(string $host, int $port, array $options = []): Server`
- `accept(): ?Session`
- `broadcast(Message $message): void`
- `shutdown(): void`

### 10.4 Session Methods
- `id(): string`
- `peer(): array`
- `send(...)`
- `close(...)`
- `stats(): array`

## 11. Error Model

Define structured errors with codes:
- `1000` Normal close.
- `1001` Going away.
- `1002` Protocol error.
- `1003` Unsupported data.
- `1004` Authentication failed.
- `1005` Timeout.
- `1006` Connection dropped.
- `1007` Payload too large.
- `1008` Rate limited.
- `1011` Internal error.

Use a PHP exception hierarchy:
- `PhoenixWireException`
- `ConnectionException`
- `ProtocolException`
- `AuthException`
- `TimeoutException`
- `BufferOverflowException`

## 12. Performance Requirements

### 12.1 Targets
- Handshake in one compact round trip.
- Frame overhead smaller than WebSocket for typical small payloads.
- No client masking.
- Zero-copy send path where possible.
- Optional batched writes.
- Minimal allocations on hot path.

### 12.2 Benchmark Metrics
Measure:
- Connection setup latency.
- Messages per second.
- P50, P95, P99 latency.
- CPU per 100k messages.
- Memory per connection.
- Wire overhead for payload sizes of 1, 8, 32, 128, 1024 bytes.

## 13. Implementation Strategy

### Phase 1: Protocol Spec
- Finalize handshake.
- Finalize frame format.
- Finalize opcode table.
- Finalize close/error codes.

### Phase 2: Native Core
- Build socket engine.
- Build parser and serializer.
- Build TLS layer.
- Build session manager.
- Build tests and fuzz harness.

### Phase 3: PHP Extension
- Create OOP interface.
- Add connection lifecycle functions.
- Expose stats and debug info.
- Add event/callback model.

### Phase 4: Tools
- CLI benchmark tool.
- Reference server.
- Packet inspector.
- Logging and tracing utilities.

## 14. Testing Requirements

### Required test suites
- Unit tests for codec and parser.
- Integration tests for client/server.
- Fuzz tests for malformed frames.
- Stress tests with many concurrent connections.
- TLS validation tests.
- Reconnect and timeout tests.
- Backpressure and queue saturation tests.

## 15. Observability

The system must expose:
- Active connections.
- Connected sessions.
- Messages sent/received.
- Queue depth.
- Bytes in/out.
- Latency percentiles.
- Error counts.
- Auth failures.
- Disconnect reasons.

## 16. Deployment Model

Support:
- Embedded PHP extension mode.
- Standalone daemon mode.
- Linux production deployment first.
- macOS and Windows compatibility as later targets.

## 17. Risks

- Custom protocols require careful documentation.
- Debugging is harder than using standard WebSocket tooling.
- Security mistakes in native code are high impact.
- PHP extension maintenance adds build complexity.

## 18. Acceptance Criteria

The project is acceptable when:
- A PHP client and server can exchange messages reliably.
- Performance is measurably better than WebSocket for small binary messages.
- The transport is stable under load.
- The protocol is versioned and extensible.
- The API is simple enough for day-to-day development.

## 19. Recommended Positioning

Position PhoenixWire as:
- A private real-time transport.
- A high-performance binary protocol for controlled environments.
- A PHP-native integration with a C/C++ transport engine.

Do not position it as a universal WebSocket replacement, because WebSocket exists specifically to satisfy browser-origin use cases and HTTP compatibility. PHP FFI can help during experimentation, but the final production path should be the native extension because FFI is not the optimal performance mechanism for this kind of system. [en.wikipedia](https://en.wikipedia.org/wiki/WebSocket)
