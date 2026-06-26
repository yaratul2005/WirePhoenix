# PhoenixWire Deployment Guide

PhoenixWire offers two primary deployment models depending on your infrastructure constraints and performance requirements.

## 1. Dedicated Server (Native Extension)

For maximum performance on dedicated servers, virtual private servers (VPS), or environments where you have root access to install PHP extensions, use the native C extension.

### Installation

1. Compile the native transport library and the PHP extension:
   ```bash
   make clean && make all
   ```
2. Enable the extension in your `php.ini` file:
   ```ini
   extension=phoenixwire.so
   ```
3. Use the `PhoenixWire\Client` and `PhoenixWire\Server` classes directly. They will be backed by the highly optimized C extension.

## 2. Shared Hosting (Hosted Daemon + Pure-PHP Client)

In shared hosting environments (e.g., cPanel, typical PHP-FPM setups), you usually cannot install native extensions or run long-lived background daemon processes. PhoenixWire supports this reality via a **Pure-PHP Stream Fallback** and a standalone **Hosted Daemon**.

### Architecture

1. **Hosted Daemon:** You run the `pw_server` daemon on a separate, small VPS (e.g., an affordable $5/mo instance) or a containerized service. This daemon is written in pure C for extreme efficiency and acts as the real-time hub.
2. **Pure-PHP Client:** Your shared-hosting PHP application uses `stream_socket_client()` via `PhoenixWire\StreamClient` to connect outwards to the daemon.

### Feature Parity: Native vs Pure-PHP

| Feature | Native Extension (`ext-phoenixwire`) | Pure-PHP (`StreamClient`) |
| :--- | :--- | :--- |
| **Connection Setup** | Zero-copy socket creation (C level) | Standard PHP `stream_socket_client` |
| **CPU Overhead** | Minimal (C level parsing & framing) | Moderate (PHP userland `pack()` / `unpack()`) |
| **TLS/SSL** | Managed via Native Core or Reverse Proxy | Built-in via PHP `tls://` wrapper or `stream_socket_enable_crypto()` |
| **Multiplexing** | Full support | Single-stream support currently |
| **Async Polling** | Full `epoll`/`select` event loop support | Blocking / semi-blocking via `stream_select` |
| **Best For:** | Servers, long-running daemons, heavy load | Shared hosting, short-lived requests, outbound clients |

### Usage

Install PhoenixWire via Composer:
```bash
composer require phoenixwire/phoenixwire
```

Use the `ClientFactory` to automatically detect the best available transport:
```php
<?php
require 'vendor/autoload.php';

use PhoenixWire\ClientFactory;

// Automatically uses ext-phoenixwire if loaded, otherwise falls back to pure-PHP stream socket
$client = ClientFactory::create([
    'auth_token' => 'your_secret_token',
    'use_tls'    => true // Explicitly enable TLS for the stream connection
]);

$client->connect('your-daemon-ip.com', 8080);
$client->send(json_encode(['action' => 'broadcast', 'data' => 'Hello World!']));

// Read response if necessary
$response = $client->receive(500); // Wait up to 500ms
```

### Transport Security (TLS)

When using the `StreamClient` in pure-PHP, passing `use_tls => true` via `ClientOptions` configures the underlying stream connection to automatically upgrade to TLS utilizing PHP's built-in `crypto_method` options.

### Hardening the Daemon

Before exposing the daemon to the public internet:
- **Authentication:** Ensure you configure a secure bearer token. The daemon inherently rejects any connections that fail to provide a valid token during the `PW_STATE_AUTHENTICATING` handshake phase.
- **TLS 1.3:** We strongly recommend placing the `pw_server` daemon behind a reverse proxy like **Nginx** or **HAProxy** to terminate TLS connections. This centralizes certificate management off the native C core.
