# PhoenixWire PHP API Usage

PhoenixWire provides a high-performance, object-oriented API for real-time binary messaging. It is distributed as a native PHP extension.

## 1. Synchronous Client

The simplest way to use PhoenixWire is via the synchronous client, which blocks until operations complete. Useful for standard CLI scripts or cron jobs.

```php
use PhoenixWire\Client;
use PhoenixWire\ClientOptions;
use PhoenixWire\Timeouts;
use PhoenixWire\Message;

$options = new ClientOptions([
    'bearerToken' => 'my-secret-token',
    'timeouts' => Timeouts::default(),
    'enableResume' => true
]);

$client = new Client($options);
$client->connect('127.0.0.1', 8080);

echo "Connected! State: " . $client->getState() . "\n";

// ... send and receive logic ...

$client->disconnect();
```

## 2. Asynchronous / Event-driven Client

For high-concurrency environments (like OpenSwoole or ReactPHP), the `AsyncClient` provides an event-driven callback model. It manages its own state machine and executes callbacks safely across reconnects.

```php
use PhoenixWire\AsyncClient;
use PhoenixWire\ClientOptions;

$client = new AsyncClient(new ClientOptions(['autoReconnect' => true]));

$client->onConnect(function($c) {
    echo "Connected to server!\n";
});

$client->onMessage(function($c, $message) {
    echo "Received: " . $message->payload . " on stream " . $message->streamId . "\n";
});

$client->onError(function($c, \Throwable $e) {
    echo "Error: " . $e->getMessage() . "\n";
});

$client->onClose(function($c) {
    echo "Connection closed. Waiting for reconnect...\n";
});

$client->connect('127.0.0.1', 8080);

// In a real event loop, you would register the socket descriptor
// For a manual tick loop:
while (true) {
    $client->tick(); // Non-blocking state update and message read
    usleep(10000);
}
```

## 3. Server Configuration

You can also run a PhoenixWire server directly from PHP, routing sessions to your application logic. The server handles multiplexing, backpressure, and heartbeat limits automatically.

```php
use PhoenixWire\Server;
use PhoenixWire\Message;

$server = new Server('0.0.0.0', 8080);

$server->onConnect(function($session) {
    echo "New client connected: " . $session->id() . "\n";
});

$server->onMessage(function($session, $message) {
    // Echo back text messages
    if ($message->type === Message::TYPE_TEXT) {
        $session->send(new Message(Message::TYPE_TEXT, "Echo: " . $message->payload));
    }
});

$server->onClose(function($session) {
    echo "Session ended: " . $session->id() . "\n";
});

echo "Starting server...\n";
$server->listen();
```

## 4. Connection Options

The `ClientOptions` object exposes granular control over connection behavior:
- `autoReconnect`: Automatically reconnect on transport loss.
- `maxReconnectAttempts`: Cap exponential backoff attempts.
- `enableResume`: Attempt to resume session ID without full re-auth.
- `bearerToken`: Auth token sent during `AUTHENTICATING` phase.
- `heartbeatIntervalMs`: Frequency of automated PING frames.

The `Timeouts` object encapsulates deadline logic:
- `connectTimeoutMs`: Allowed time for socket connect.
- `handshakeTimeoutMs`: Allowed time for server to ACCEPT protocol and capabilities.
- `authTimeoutMs`: Allowed time for server to validate tokens.
- `idleTimeoutMs`: Grace period before silent connection is forcefully closed.
