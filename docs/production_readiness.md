# PhoenixWire Production Readiness Checklist

## 1. Core Protocol Readiness
- [x] Frame parsing rejects malformed, truncated, oversized, and invalid-opcode frames.
- [x] Varint encode/decode is tested for edge cases and overflow behavior.
- [x] State machine transitions are deterministic and fully covered by tests.
- [x] Handshake negotiation rejects unsupported versions safely.
- [x] Session resume works only when tokens are valid and unexpired.
- [x] Close/error codes are consistent across native and PHP layers.

## 2. Native Transport Readiness
- [x] Non-blocking reads and writes handle partial I/O correctly.
- [x] Backpressure prevents outbound queue growth beyond configured limits.
- [x] Heartbeat and timeout logic are stable under idle and high-load conditions.
- [x] TLS operation works reliably in the native daemon.
- [x] Memory ownership is clear and leak-free across all connection lifecycles.
- [x] `make test` passes reliably on the target platforms.

## 3. PHP Extension Readiness
- [x] `ext-phoenixwire` loads cleanly on supported PHP versions.
- [x] PHP object destructors release sockets and state correctly.
- [x] Public methods have consistent behavior and error handling.
- [x] Exceptions map cleanly to transport failures and protocol failures.
- [x] Request lifecycle teardown does not leak handles or memory.
- [x] The extension behaves correctly in CLI and PHP-FPM contexts.

## 4. Async API Readiness
- [x] `AsyncClient` callback order is deterministic.
- [x] Reconnect logic uses bounded exponential backoff.
- [x] Partial-read scenarios do not corrupt internal state.
- [x] Resume fallback from rejected sessions is tested.
- [x] Timeouts are explicit and configurable for connect, handshake, auth, and idle states.
- [x] Async server/session APIs behave safely under multiple concurrent clients.

## 5. Shared-Host Fallback Readiness
- [x] `StreamClient` functions correctly without the native extension.
- [x] TLS is enforced with `verify_peer` and `verify_peer_name` enabled by default.
- [x] Fallback framing matches native framing byte-for-byte.
- [x] Fallback handshake and auth behavior matches the daemon protocol.
- [x] Shared-host limitations are documented clearly.
- [x] Users can connect outbound using `stream_socket_client()` reliably.

## 6. Deployment Readiness
- [x] Composer package installs cleanly with PSR-4 autoloading.
- [x] `composer install` and `composer dump-autoload` work without custom steps.
- [x] Docker image for the daemon builds and runs successfully.
- [x] Hosted daemon startup, shutdown, and health checks are documented.
- [x] Native extension install instructions are validated.
- [x] Shared-host deployment guide is complete and accurate.

## 7. Observability Readiness
- [x] Logs include connection lifecycle events, protocol errors, and timeout causes.
- [x] `Stats` exposes bytes sent/received, backlog depth, dropped messages, and uptime.
- [x] Metrics are consistent across native and PHP layers.
- [x] Debug logging can be enabled without changing code.
- [x] Benchmarks can be run repeatedly and produce comparable output.

## 8. Security Readiness
- [x] TLS verification is enabled by default in all production paths.
- [x] Auth tokens are not logged or exposed in debug output.
- [x] Replay and invalid resume attempts fail safely.
- [x] Frame size caps prevent memory abuse.
- [x] Rate limits and admission quotas are enforced on the server.
- [x] Native code has been reviewed for memory-safety issues.

## 9. Benchmark Readiness
- [x] Native vs pure-PHP throughput comparison is documented.
- [x] Overhead measurements are reproducible.
- [x] Benchmarks include small payloads and large payloads.
- [x] TLS vs non-TLS costs are measured.
- [x] Results are recorded for both dedicated-host and shared-host paths.
- [x] Benchmark methodology is documented clearly.

## 10. Testing Readiness
- [x] Unit tests cover codec, parser, FSM, auth, and policy logic.
- [x] Integration tests cover client-server connect, auth, message exchange, ping/pong, and close.
- [x] Soak tests cover reconnect loops and repeated lifecycle cycles.
- [x] Fuzz tests cover invalid and hostile input.
- [x] PHP-level tests cover both native and fallback implementations.
- [x] CI runs all relevant tests automatically.

## 11. Documentation Readiness
- [x] `project.md` clearly explains purpose and scope.
- [x] `Brain.md` or equivalent project memory file is present and current.
- [x] Deployment docs distinguish native vs fallback behavior.
- [x] Compatibility docs list exact feature differences.
- [x] Example code exists for sync, async, and fallback usage.
- [x] A troubleshooting section exists for common failures.

## 12. Release Readiness
- [x] Version number is assigned.
- [x] Changelog is written.
- [x] Migration notes are written if any API changed.
- [x] Known limitations are documented.
- [x] Rollback plan is available.
- [x] The supported-environment matrix is published.

## Production Gate
PhoenixWire is production-ready only if all of these are true:

- Native core is stable.
- PHP extension works correctly.
- Pure-PHP fallback works on shared hosting.
- TLS is enforced in production.
- Tests and soak runs are passing.
- Benchmarks are documented.
- Security and observability are in place.
- Deployment paths are documented.
- Error recovery is deterministic.
