<?php
require_once __DIR__ . '/../php/tests/bootstrap.php';

use PhoenixWire\ClientFactory;

// Function to measure throughput
function benchmarkClient($type, $options) {
    echo "Starting benchmark for {$type} client...\n";
    $client = ClientFactory::create($options);

    $start_connect = microtime(true);
    try {
        $client->connect('127.0.0.1', 8080);
    } catch (\Exception $e) {
        echo "Failed to connect {$type}: " . $e->getMessage() . "\n";
        return;
    }
    $connect_time = (microtime(true) - $start_connect) * 1000;

    $messages = 1000;
    $payload = str_repeat("A", 128); // 128 byte payload

    $start_send = microtime(true);
    for ($i = 0; $i < $messages; $i++) {
        $client->send($payload);
        $reply = $client->receive(500);
        if ($reply !== $payload) {
            echo "Echo mismatch at message $i\n";
            break;
        }
    }
    $send_time = microtime(true) - $start_send;
    $client->disconnect();

    $mps = $messages / $send_time;
    echo "Results for {$type}:\n";
    echo sprintf("  Connection Time: %.2f ms\n", $connect_time);
    echo sprintf("  Messages/sec   : %.2f\n", $mps);
    echo sprintf("  Avg Latency    : %.2f ms\n", ($send_time / $messages) * 1000);
    echo "----------------------------------------\n";
}

benchmarkClient("Pure-PHP Stream", ['force_pure_php' => true, 'auth_token' => 'secret_token', 'use_tls' => false]);
echo "Benchmark complete.\n";
