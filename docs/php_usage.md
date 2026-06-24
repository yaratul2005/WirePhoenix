# PhoenixWire PHP API Usage

PhoenixWire provides a high-performance, object-oriented API for real-time binary messaging. It is distributed as a native PHP extension.

## 1. Synchronous Client

The simplest way to use PhoenixWire is via the synchronous client, which blocks until operations complete.

```php
use PhoenixWire\Client;
use PhoenixWire\ClientOptions;
use PhoenixWire\Message;

$options = new ClientOptions([
    'bearerToken' => 'my-secret-token',
    'timeoutMs' => 2000
]);

$client = new Client($options);
$client->connect('127.0.0.1', 8080);

echo "Connected! State: " . $client->getState() . "\n";

$client->disconnect();
```

## 2. Asynchronous / Event-driven Client

For high-concurrency environments (like OpenSwoole or ReactPHP), the `AsyncClient` provides an event-driven callback model.

```php
use PhoenixWire\AsyncClient;

$client = new AsyncClient();

$client->onConnect(function() {
    echo "Connected to server!\n";
});

$client->onMessage(function($message) {
    echo "Received: " . $message->payload . "\n";
});

$client->onClose(function() {
    echo "Connection closed.\n";
});

$client->connect('127.0.0.1', 8080);

// In a real event loop, you would register the socket descriptor
// For a manual loop:
while ($client->getState() !== Client::STATE_CLOSED) {
    $client->tick();
    usleep(10000);
}
```

## 3. Server Configuration

You can also run a PhoenixWire server directly from PHP, routing sessions to your application logic.

```php
use PhoenixWire\Server;

$server = new Server('0.0.0.0', 8080);

$server->onConnect(function($session) {
    echo "New client connected: " . $session->id() . "\n";
});

$server->onMessage(function($session, $message) {
    // Echo back
    $session->send($message->payload);
});

echo "Starting server...\n";
$server->listen();
```

## 4. Connection Options

- `timeoutMs`: Time to wait for handshake/auth before dropping.
- `autoReconnect`: Automatically reconnect on transport loss.
- `enableResume`: Attempt to resume session ID without full re-auth.
- `bearerToken`: Auth token sent during `AUTHENTICATING` phase.
- `heartbeatIntervalMs`: Frequency of automated PING frames.
