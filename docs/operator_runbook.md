# PhoenixWire Operator Runbook

## 1. Deployment
**Native Daemon:** Run via systemd or Docker.
```bash
# systemd example
systemctl start phoenixwire-daemon
systemctl enable phoenixwire-daemon
```

**PHP Configuration:**
Ensure `extension=phoenixwire.so` is loaded in `php.ini`.

## 2. Health Checks
- **Transport Level:** Dispatch a `PING` (0x03) opcode. The server must respond with `PONG` (0x04) within the heartbeat interval (default: 30s).
- **Daemon Level:** Ensure process `pw_server` is bound to the target TCP port (e.g., 8080).

## 3. Failure Recovery
- **High Memory Usage:** Investigate slow consumers. The daemon limits frame queues, but if `max_queue_size` is too high, OOM killer may intervene. Restart daemon if necessary.
- **TLS Rotation:** When rotating certificates, the daemon must be gracefully reloaded (SIGHUP) to read the new `cert.pem` and `key.pem`. Active connections are preserved; new connections use the new cert.
- **Extension Segfault:** Collect core dump and check `dmesg`. Temporarily toggle `ClientFactory` to `force_pure_php=true` to restore service while debugging the native extension.

## 4. Log Tracing
- Native daemon logs to stdout (or syslog if configured).
- PHP extension errors use the standard `php_error_docref` and appear in the PHP error logs.
