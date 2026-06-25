<div align="center">
  <img src="assets/phoenix-logo.png" alt="PhoenixWire Logo" width="300" />
</div>

# PhoenixWire Transport

**PhoenixWire is a custom, high-performance real-time transport protocol and PHP extension for applications that need lower overhead than WebSocket and do not require browser interoperability.**

Designed for server-to-server, mobile, IoT, gaming, and internal application traffic, PhoenixWire focuses on compact framing, predictable latency, and native integration.

<div align="center">
  <strong>OWNER: YASER AHMMED RATUL from BANGLADESH, PORTFOLIO: https://yaratul.com</strong>
</div>

---

## ⚡ Overview

PhoenixWire bridges the gap between raw TCP sockets and heavyweight application protocols like HTTP/WebSockets. It provides a structured, multiplexed, and secure message-oriented transport, written in highly optimized native C and exposed via a robust PHP extension.

### Primary Goals
- **Minimize Overhead:** Extremely compact binary framing and low connection setup cost.
- **Native Performance:** Core network and parsing operations are written in native C/C++ to avoid PHP user-space bottlenecks.
- **PHP-Friendly API:** A clean, object-oriented API in PHP for ease of use in production environments.
- **Robustness:** Built-in backpressure, heartbeat mechanisms, and strict state management.

### Non-Goals
- Browser compatibility (no RFC 6455 drop-in replacement).
- Human-readable wire format by default.
- Pure PHP implementation of the transport core.

---

## 🏗 System Architecture

PhoenixWire is composed of the following layers:

1. **Native Transport Core (C):** Handles non-blocking TCP socket I/O, state machines, frame serialization/deserialization (varints, opcodes), backpressure, and heartbeat management.
2. **PHP Extension API:** Exposes the C core via Zend object handlers, providing an elegant `Client`, `Server`, and `Session` interface to PHP userland.
3. **Protocol Codec:** Implements custom binary framing with specific opcodes (`DATA_BINARY`, `PING`, `AUTH`, etc.).
4. **Security/Authentication Layer:** Built-in bearer token authentication logic at the transport handshake level.

---

## 🚀 Usability & Examples

The extension provides a synchronous and asynchronous API for developers.

### Server Example

```php
<?php
$server = new PhoenixWire\Server("0.0.0.0", 8080);
echo "Listening on port 8080...\n";

while (true) {
    $session = $server->accept();
    if ($session) {
        echo "Accepted connection: " . $session->id() . "\n";

        while (true) {
            $msg = $session->receive(100); // 100ms timeout
            if ($msg) {
                if ($msg->isClose()) {
                    break;
                }
                echo "Received: " . $msg->getPayload() . "\n";
                $session->send("Echo: " . $msg->getPayload());
            }
        }
        $session->close();
    }
    usleep(10000);
}
```

### Async Client Example

```php
<?php
$client = new PhoenixWire\AsyncClient("127.0.0.0", 8080);
$client->connect();

$client->onMessage(function($payload) {
    echo "Server said: " . $payload . "\n";
});

$client->send("Hello, PhoenixWire!");

// Process events non-blockingly
while ($client->isConnected()) {
    $client->tick();
    usleep(10000);
}
```

---

## 📦 Protocol Details

### Framing
Frames consist of a compact header and a variable payload.

| Field | Description |
|---|---|
| Magic (2 bytes) | `PW` |
| Opcode (1 byte) | Denotes message type (Data, Ping, Close, Auth) |
| Length | Varint-encoded payload length |
| Payload | Variable byte array |

### Connection Lifecycle
- `CONNECTING`
- `HANDSHAKING`
- `AUTHENTICATING`
- `READY`
- `DRAINING`
- `CLOSED`

---

## 🛠 Building from Source

Ensure you have `php-dev` and a C compiler installed.

```bash
# 1. Build the native library and extension
make clean
make all

# 2. Run tests
make test
```

To load the extension in your PHP scripts, ensure `phoenixwire.so` is loaded in your `php.ini` or use `php -d extension=./php/modules/phoenixwire.so`.

---

## 📈 Observability & Benchmarks

PhoenixWire is designed for scale. A `tools/benchmark.php` script is included to measure messages per second, latency, and CPU usage. Built-in metrics (accessible via `$session->stats()`) allow for runtime observability.

---

## ⚖️ Acceptance & Positioning

PhoenixWire is specifically positioned as a high-performance binary protocol for internal services, edge devices, and server-to-server communications in PHP ecosystems, avoiding the overhead of HTTP/WebSocket layers.

---
