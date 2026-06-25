<?php
require_once __DIR__ . '/bootstrap.php';

use PhoenixWire\AsyncClient;
use PhoenixWire\ClientOptions;
use PhoenixWire\Policy;

class PolicyTest {

    public function testExponentialBackoff() {
        $policy = new Policy(maxReconnectAttempts: 5, initialBackoffMs: 100, maxBackoffMs: 1000, backoffMultiplier: 2.0);
        $options = new ClientOptions(['autoReconnect' => true]);
        $options->policy = $policy;

        $client = new AsyncClient($options);

        // Mock method to access private calculateBackoff via Reflection
        $reflection = new \ReflectionClass(AsyncClient::class);
        $method = $reflection->getMethod('calculateBackoff');
        $method->setAccessible(true);
        $reconnectProp = $reflection->getProperty('reconnectAttempts');
        $reconnectProp->setAccessible(true);

        // Attempt 0
        $reconnectProp->setValue($client, 0);
        $backoff = $method->invoke($client);
        if ($backoff !== 100) throw new \Exception("Expected 100, got $backoff");

        // Attempt 1
        $reconnectProp->setValue($client, 1);
        $backoff = $method->invoke($client);
        if ($backoff !== 200) throw new \Exception("Expected 200, got $backoff");

        // Attempt 2
        $reconnectProp->setValue($client, 2);
        $backoff = $method->invoke($client);
        if ($backoff !== 400) throw new \Exception("Expected 400, got $backoff");

        // Attempt 3
        $reconnectProp->setValue($client, 3);
        $backoff = $method->invoke($client);
        if ($backoff !== 800) throw new \Exception("Expected 800, got $backoff");

        // Attempt 4 (capped)
        $reconnectProp->setValue($client, 4);
        $backoff = $method->invoke($client);
        if ($backoff !== 1000) throw new \Exception("Expected max 1000, got $backoff");
    }

    public function testRetryCaps() {
        $policy = new Policy(maxReconnectAttempts: 2, initialBackoffMs: 10, maxBackoffMs: 50, backoffMultiplier: 2.0);
        $options = new ClientOptions(['autoReconnect' => true]);
        $options->policy = $policy;

        $client = new AsyncClient($options);

        // Transition to READY, then CLOSED
        $client->_setMockState(\PhoenixWire\Client::STATE_READY);
        $client->tick(); // triggers onConnect

        $client->_setMockState(\PhoenixWire\Client::STATE_CLOSED);

        // First tick after close should schedule reconnect
        $client->tick();

        $reflection = new \ReflectionClass(AsyncClient::class);
        $nextReconnectProp = $reflection->getProperty('nextReconnectTime');
        $nextReconnectProp->setAccessible(true);
        $reconnectProp = $reflection->getProperty('reconnectAttempts');
        $reconnectProp->setAccessible(true);

        $nextTime = $nextReconnectProp->getValue($client);
        if ($nextTime === 0.0) throw new \Exception("Reconnect not scheduled");

        // Advance time to force reconnect execution
        $nextReconnectProp->setValue($client, microtime(true) - 1.0);
        $client->tick(); // Should execute reconnect 1

        if ($reconnectProp->getValue($client) !== 1) throw new \Exception("Expected 1 reconnect attempt");

        // Second reconnect cycle
        $client->_setMockState(\PhoenixWire\Client::STATE_CLOSED);
        $client->tick(); // schedule 2
        $nextReconnectProp->setValue($client, microtime(true) - 1.0);
        $client->tick(); // execute 2

        if ($reconnectProp->getValue($client) !== 2) throw new \Exception("Expected 2 reconnect attempts");

        // Third cycle should hit cap
        $client->_setMockState(\PhoenixWire\Client::STATE_CLOSED);
        $client->tick(); // Try to schedule 3

        $nextTime = $nextReconnectProp->getValue($client);
        if ($nextTime !== 0.0) throw new \Exception("Reconnect scheduled beyond maxAttempts limit");
    }
}
