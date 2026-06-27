# PhoenixWire Compatibility & Benchmarks

This document outlines the expected compatibility matrices and benchmark baselines for PhoenixWire across its supported deployment models.

## Compatibility Matrix

### Transport Methods

| Deployment Model | Native FFI/Ext (`Client`) | Pure-PHP Fallback (`StreamClient`) | Protocol Support |
| :--- | :---: | :---: | :---: |
| **Dedicated Server / Root VPS** | ✅ Best Performance | ✅ Functional | Complete |
| **Shared Hosting (cPanel, etc)** | ❌ Unsupported | ✅ Standard fallback | Complete |
| **Edge Devices (IoT)** | ✅ C Core Available | ❌ Usually N/A | Complete |

### TLS Support

| Mode | Configuration | Mechanism |
| :--- | :--- | :--- |
| **Native Extension** | `tls://` disabled locally | Depends on a reverse proxy (e.g., Nginx/HAProxy) handling termination, or native OpenSSL build flag. |
| **Stream Fallback** | `use_tls => true` | Upgrades standard socket to TLS (`STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT | STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT`) enforcing strict `verify_peer` logic natively in PHP. |

## Comparative Benchmarks

*Note: Benchmarks vary wildly based on virtualization latency and network topology. These represent idealized local loopback targets.*

**Test Environment:** Standard x86_64 VPS (1vCPU, 2GB RAM)
**Payload:** 128 byte echo loop

| Metric | Native C Extension | Pure-PHP Stream Client |
| :--- | :--- | :--- |
| **Connection Time** | < 1ms | 2 - 5ms |
| **Max Throughput** | ~50,000+ msg/sec | ~15,000 msg/sec |
| **Avg Latency** | 0.05 ms | 0.20 ms |
| **CPU Load / 1k msg** | Minimal | Noticeable (varint pack/unpack logic) |

### Analyzing the Trade-offs

- The **Native Extension** shines under persistent connection pools or high-frequency polling where binary frame serialization overhead and context switching in PHP would bottleneck the application. It maps the memory structures zero-copy.
- The **Pure-PHP Stream Client** takes a CPU hit processing `pack` and `unpack` on each frame header but operates securely over TLS and requires absolutely zero custom infrastructure. It is fully viable for typical low-frequency WebSocket-like replacements (e.g. chat messages, status updates).
