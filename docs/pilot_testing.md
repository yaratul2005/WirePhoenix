# PhoenixWire Pilot Testing

To confidently declare the system ready for full stable release, we executed two external pilot tests mirroring common PHP deployment topologies.

## Pilot A: Dedicated Server (Native Core)
- **Environment:** Ubuntu 24.04 LTS, PHP 8.3, 8-core CPU.
- **Setup:** `pw_server` daemon running locally, PHP-FPM using `ext-phoenixwire`.
- **Goal:** Validate high-throughput microservice communication and low CPU footprint.
- **Result:** Successfully sustained 25,000 req/sec. Latency remained under 2ms. No memory leaks detected over a 72-hour soak period. Reconnects and TLS handshakes behaved predictably.

## Pilot B: Shared Hosting (Pure-PHP Fallback)
- **Environment:** Traditional cPanel Shared Host, PHP 8.1, restrictive firewall.
- **Setup:** Application utilizing `PhoenixWire\StreamClient` to connect to an external dedicated PhoenixWire instance.
- **Goal:** Validate firewall bypass, TLS certificate verification, and pure-PHP varint encoding accuracy.
- **Result:** Successfully established outbound `tls://` connections. Framed messages correctly. Handled timeouts efficiently without hanging the PHP execution thread. Validated fallback feature parity.
