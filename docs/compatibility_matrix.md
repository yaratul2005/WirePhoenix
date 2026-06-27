# PhoenixWire Compatibility Matrix

## PHP Versions
| PHP Version | Native Extension (`ext-phoenixwire`) | Pure-PHP Fallback (`StreamClient`) | Status |
|---|---|---|---|
| 7.4 | Supported | Supported | Security Fixes Only |
| 8.0 | Supported | Supported | EOL |
| 8.1 | Supported | Supported | Active |
| 8.2 | Supported | Supported | Active |
| 8.3 | Supported | Supported | Active (Recommended) |

## Hosting Environments
| Environment | Mode | Performance | Notes |
|---|---|---|---|
| Dedicated Server | Native Extension + Daemon | High | Zero-copy socket polling, epoll backends. |
| Containerized (K8s/Docker) | Native Extension | High | Requires shared volume/network for daemon. |
| Shared Hosting (cPanel) | Pure-PHP Fallback | Moderate | Limited by `stream_socket_client` and PHP userland execution. |

## Operating Systems
| OS | Native Core | PHP Extension | pure-PHP | Notes |
|---|---|---|---|---|
| Linux (Ubuntu/Debian/RHEL) | Fully Supported | Fully Supported | Fully Supported | Primary target platform. |
| macOS | Best Effort | Best Effort | Fully Supported | Good for local development. |
| Windows | Untested | Untested | Fully Supported | Use WSL2 for native core. |
