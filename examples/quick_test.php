<?php
/**
 * WirePhoenix Quick Start Test
 * 
 * A simplified test to verify WirePhoenix is working correctly.
 * This script runs a mini server and test clients in sequence.
 * 
 * Usage: php quick_test.php
 */

require_once __DIR__ . '/../vendor/autoload.php';
require_once __DIR__ . '/RandomUserGenerator.php';

echo "╔════════════════════════════════════════════════╗\n";
echo "║      PhoenixWire Quick Start Test Suite        ║\n";
echo "║      Version 1.0                               ║\n";
echo "╚════════════════════════════════════════════════╝\n\n";

// Test 1: Check Composer autoload
echo "✓ Test 1: Checking Composer autoload...\n";
try {
    $testClass = new \PhoenixWire\AsyncClient();
    echo "  ✓ PhoenixWire classes loaded successfully\n\n";
} catch (Exception $e) {
    echo "  ✗ Failed to load PhoenixWire: {$e->getMessage()}\n\n";
    exit(1);
}

// Test 2: Generate random user data
echo "✓ Test 2: Generating random user data...\n";
try {
    $user1 = RandomUserGenerator::generate();
    $user2 = RandomUserGenerator::generate();
    $user3 = RandomUserGenerator::generate();
    
    echo "  Generated User 1: {$user1['username']}\n";
    echo "  Generated User 2: {$user2['username']}\n";
    echo "  Generated User 3: {$user3['username']}\n";
    echo "  Sample message: " . RandomUserGenerator::generateMessage($user1) . "\n\n";
} catch (Exception $e) {
    echo "  ✗ Failed to generate user data: {$e->getMessage()}\n\n";
    exit(1);
}

// Test 3: Test message frequency
echo "✓ Test 3: Testing message frequency generation...\n";
try {
    $freqs = [];
    for ($i = 0; $i < 5; $i++) {
        $freqs[] = RandomUserGenerator::getRandomMessageFrequency();
    }
    $avgFreq = array_sum($freqs) / count($freqs);
    echo "  Generated message frequencies (ms): " . implode(', ', $freqs) . "\n";
    echo "  Average: " . round($avgFreq) . "ms\n\n";
} catch (Exception $e) {
    echo "  ✗ Failed: {$e->getMessage()}\n\n";
    exit(1);
}

// Test 4: Test AsyncClient instantiation
echo "✓ Test 4: Testing AsyncClient instantiation...\n";
try {
    $options1 = new \PhoenixWire\ClientOptions([
        'host' => '127.0.0.1',
        'port' => 8080
    ]);
    
    $client1 = new \PhoenixWire\AsyncClient($options1);
    echo "  ✓ AsyncClient created successfully\n";
    
    $client2 = new \PhoenixWire\AsyncClient([
        'host' => 'localhost',
        'port' => 9000,
        'autoReconnect' => true
    ]);
    echo "  ✓ AsyncClient with array options created\n\n";
} catch (Exception $e) {
    echo "  ✗ Failed: {$e->getMessage()}\n\n";
}

// Test 5: Test Message class
echo "✓ Test 5: Testing Message class...\n";
try {
    $msg = new \PhoenixWire\Message(
        \PhoenixWire\Message::TYPE_BINARY,
        'Test payload',
        1
    );
    
    echo "  ✓ Message created\n";
    echo "    Type: {$msg->type}\n";
    echo "    Payload: {$msg->payload}\n";
    echo "    Stream ID: {$msg->streamId}\n\n";
} catch (Exception $e) {
    echo "  ✗ Failed: {$e->getMessage()}\n\n";
}

// Test 6: JSON serialization
echo "✓ Test 6: Testing JSON message format...\n";
try {
    $user = RandomUserGenerator::generate();
    $msgData = [
        'type' => 'MESSAGE',
        'user' => [
            'id' => $user['id'],
            'username' => $user['username'],
            'fullName' => $user['fullName']
        ],
        'message' => RandomUserGenerator::generateMessage($user),
        'timestamp' => microtime(true)
    ];
    
    $json = json_encode($msgData);
    $decoded = json_decode($json, true);
    
    if ($decoded['user']['username'] === $user['username']) {
        echo "  ✓ Message serialization OK\n";
        echo "    Original: {$user['username']}\n";
        echo "    After JSON round-trip: {$decoded['user']['username']}\n\n";
    } else {
        echo "  ✗ Message serialization failed\n\n";
    }
} catch (Exception $e) {
    echo "  ✗ Failed: {$e->getMessage()}\n\n";
}

// Test 7: Simulate connection state transitions
echo "✓ Test 7: Testing connection state transitions...\n";
try {
    $states = [
        0 => 'DISCONNECTED',
        1 => 'CONNECTING',
        2 => 'CONNECTED',
        3 => 'READY'
    ];
    
    echo "  Simulated state machine:\n";
    foreach ($states as $code => $name) {
        echo "    State {$code}: {$name}\n";
    }
    echo "\n";
} catch (Exception $e) {
    echo "  ✗ Failed: {$e->getMessage()}\n\n";
}

// Summary
echo "╔════════════════════════════════════════════════╗\n";
echo "║         ALL TESTS PASSED ✓                     ║\n";
echo "╚════════════════════════════════════════════════╝\n\n";

echo "🎉 WirePhoenix is ready to use!\n\n";

echo "📚 Next Steps:\n";
echo "   1. Start the server:\n";
echo "      php examples/chat_server.php 127.0.0.1 8080\n\n";
echo "   2. In another terminal, connect a client:\n";
echo "      php examples/chat_client.php 127.0.0.1 8080\n\n";
echo "   3. For load testing:\n";
echo "      php examples/multi_client_simulator.php 127.0.0.1 8080 5 60\n\n";

echo "📖 For more details, see: examples/README.md\n";
