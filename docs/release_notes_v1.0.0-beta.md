# Release Notes: v1.0.0-beta

**Release Date:** 2026-06-27
**Status:** Beta (Production-Ready Preview)

PhoenixWire v1.0.0-beta is our first major milestone, introducing a high-performance, non-browser real-time transport protocol optimized for PHP environments.

## Supported Scenarios
- **Server-to-Server RPC:** Low-overhead binary messaging between microservices.
- **Edge / IoT Device Communication:** Devices communicating via compact varint frames.
- **Shared Hosting Integration:** Fallback pure-PHP clients securely connecting to external daemons via `tls://`.
- **Session Resumption:** Token-based reconnects mitigating intermittent network loss.

## Unsupported Scenarios
- **Browser Clients:** We do NOT support RFC 6455 (WebSocket). Browsers cannot natively connect to PhoenixWire without a gateway.
- **Human-Readable Text Protocol:** Wire-level is entirely binary varint frames. (Though payloads can be JSON/UTF-8).
- **Infinite Buffering:** OOM prevention is aggressive. Unacked queues will forcibly drop or disconnect.

## Features Included
- Native C/C++ core with epoll-based TCP processing.
- Built-in Bearer Auth and Handshake phase limits.
- Full PHP Extension (`phoenixwire.so`) and pure-PHP `StreamClient` fallback.
