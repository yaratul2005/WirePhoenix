<?php
namespace PhoenixWire\Tests;

use PhoenixWire\ClientFactory;
use PhoenixWire\StreamClient;

// This acts as our integration harness to verify the pure-PHP client logic specifically against the C daemon
require_once __DIR__ . '/bootstrap.php';

$client = ClientFactory::create(['force_pure_php' => true, 'auth_token' => 'secret_token']);
echo "Testing StreamClient connection logic...\n";

try {
    $client->connect('127.0.0.1', 8080);
    echo "Connected successfully to pw_server.\n";

    $client->send("Echo Test String");
    echo "Sent message.\n";

    $reply = $client->receive(1000);
    if ($reply === "Echo Test String") {
        echo "Received exact echo back successfully.\n";
    } else {
        echo "Failed echo test. Got: $reply\n";
    }

    $client->disconnect();
    echo "Gracefully disconnected.\n";
} catch (\Exception $e) {
    echo "Failed StreamClient test: " . $e->getMessage() . "\n";
}
