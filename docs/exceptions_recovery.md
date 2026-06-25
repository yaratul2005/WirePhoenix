# PhoenixWire Exception and Recovery Mapping

In an asynchronous messaging environment, defining how the system reacts to specific error conditions is critical for resilience.

## Exception Types

PhoenixWire provides a structured hierarchy for errors during transport and protocol negotiations.

| Exception Class | Description | Recovery Strategy |
|---|---|---|
| `PhoenixWire\Exception` | Base class for all PhoenixWire errors. | General catch-all; usually indicates a terminal logic error in userland. |
| `PhoenixWire\ConnectionException` | Emitted when the underlying TCP socket fails to connect or is abruptly dropped. | **Retryable**. Triggers exponential backoff auto-reconnect if enabled. |
| `PhoenixWire\TimeoutException` | Emitted when a phase deadline (e.g., handshake, auth) is exceeded. | **Retryable**. Triggers auto-reconnect, but increments dropped counters. |
| `PhoenixWire\ProtocolException` | Emitted on malformed frames, bad magic bytes, or version mismatch. | **Force Close**. Indicates a severe incompatibility or MITM attack. Closes connection, resets state, disables immediate auto-retry. |
| `PhoenixWire\AuthException` | Emitted when the server rejects a token or session resume fails. | **Session Failure**. The connection is closed. Can optionally retry if the token is refreshed by the user application. Auto-reconnect uses backoff. |

## AsyncClient Behavior

When using `AsyncClient::tick()`, exceptions are gracefully routed to the `$client->onError(callable)` hook.

1. **Transient Errors** (`ConnectionException`, `TimeoutException`) will automatically trigger the backoff scheduler if `autoReconnect` is true. The user does not need to manually call `connect()` again.
2. **Terminal Errors** (`ProtocolException`) will immediately drop the connection, fire `onClose`, and halt the reconnect scheduler until explicitly overridden.
