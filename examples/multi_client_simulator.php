<?php
/**
 * WirePhoenix Multi-Client Simulator
 * 
 * Spawns multiple concurrent clients to test server capabilities
 * 
 * Usage: php multi_client_simulator.php [host] [port] [num_clients] [duration_seconds]
 * Example: php multi_client_simulator.php 127.0.0.1 8080 5 60
 */

require_once __DIR__ . '/../vendor/autoload.php';
require_once __DIR__ . '/RandomUserGenerator.php';

use PhoenixWire\AsyncClient;
use PhoenixWire\ClientOptions;

$host = $GLOBALS['argv'][1] ?? '127.0.0.1';
$port = (int)($GLOBALS['argv'][2] ?? 8080);
$numClients = (int)($GLOBALS['argv'][3] ?? 5);
$duration = (int)($GLOBALS['argv'][4] ?? 60);

echo "╔════════════════════════════════════════════════╗\n";
echo "║   PhoenixWire Multi-Client Load Test Simulator ║\n";
echo "║   Version 1.0                                  ║\n";
echo "╚════════════════════════════════════════════════╝\n\n";

echo "📊 Test Configuration:\n";
echo "   Server: {$host}:{$port}\n";
echo "   Clients: {$numClients}\n";
echo "   Duration: {$duration}s\n";
echo "   Start time: " . date('Y-m-d H:i:s') . "\n\n";

$clients = [];
$stats = [
    'connected' => 0,
    'totalMessagesSent' => 0,
    'totalMessagesReceived' => 0,
    'errors' => 0
];

$startTime = microtime(true);
$endTime = $startTime + $duration;

// Initialize all clients
echo "🚀 Spawning {$numClients} clients...\n";

for ($i = 1; $i <= $numClients; $i++) {
    $userData = RandomUserGenerator::generate();
    
    try {
        $options = new ClientOptions([
            'host' => $host,
            'port' => $port,
            'autoReconnect' => true,
            'token' => $userData['id']
        ]);
        
        $client = new AsyncClient($options);
        
        // Store client state
        $clientState = [
            'client' => $client,
            'userData' => $userData,
            'connected' => false,
            'messagesSent' => 0,
            'messagesReceived' => 0,
            'nextMessageTime' => microtime(true) + RandomUserGenerator::getRandomMessageFrequency(),
            'connectTime' => null
        ];
        
        // Connection handler
        $client->onConnect(function () use (&$clientState, &$stats) {
            $clientState['connected'] = true;
            $clientState['connectTime'] = microtime(true);
            $stats['connected']++;
        });
        
        // Message handler
        $client->onMessage(function ($payload) use (&$clientState, &$stats) {
            $clientState['messagesReceived']++;
            $stats['totalMessagesReceived']++;
        });
        
        // Error handler
        $client->onError(function ($error) use (&$stats) {
            $stats['errors']++;
        });
        
        // Close handler
        $client->onClose(function () use (&$clientState, &$stats) {
            if ($clientState['connected']) {
                $stats['connected']--;
            }
            $clientState['connected'] = false;
        });
        
        $clients[$i] = $clientState;
        echo "   [{$i}] {$userData['username']} initialized\n";
        
    } catch (Exception $e) {
        echo "   [{$i}] ❌ Failed to initialize: {$e->getMessage()}\n";
        $stats['errors']++;
    }
    
    usleep(100000); // 100ms between client initialization
}

echo "\n✨ All clients initialized. Starting simulation...\n";
echo "═══════════════════════════════════════════════════\n\n";

$lastStatsTime = $startTime;

// Main event loop
while (microtime(true) < $endTime) {
    $currentTime = microtime(true);
    
    // Process all clients
    foreach ($clients as $idx => &$clientState) {
        $client = $clientState['client'];
        $userData = $clientState['userData'];
        
        try {
            // Tick the client
            $client->tick();
            
            // Auto-send messages at random intervals
            if ($clientState['connected'] && $currentTime >= $clientState['nextMessageTime']) {
                $chatMessage = RandomUserGenerator::generateMessage($userData);
                
                $msg = json_encode([
                    'type' => 'MESSAGE',
                    'text' => $chatMessage,
                    'user' => [
                        'username' => $userData['username'],
                        'id' => $userData['id']
                    ],
                    'timestamp' => microtime(true)
                ]);
                
                $client->send($msg);
                $clientState['messagesSent']++;
                $stats['totalMessagesSent']++;
                
                // Set next message time
                $clientState['nextMessageTime'] = $currentTime + RandomUserGenerator::getRandomMessageFrequency();
            }
        } catch (Exception $e) {
            $stats['errors']++;
        }
    }
    
    // Print stats every 10 seconds
    if ($currentTime - $lastStatsTime >= 10) {
        $elapsedTime = round($currentTime - $startTime);
        $remainingTime = round($endTime - $currentTime);
        
        echo "📊 Status (T+{$elapsedTime}s, {$remainingTime}s remaining):\n";
        echo "   Connected Clients: {$stats['connected']}/{$numClients}\n";
        echo "   Total Messages Sent: {$stats['totalMessagesSent']}\n";
        echo "   Total Messages Received: {$stats['totalMessagesReceived']}\n";
        echo "   Errors: {$stats['errors']}\n";
        
        // Per-client stats
        $avgSent = $stats['totalMessagesSent'] > 0 ? round($stats['totalMessagesSent'] / $numClients, 2) : 0;
        $avgReceived = $stats['totalMessagesReceived'] > 0 ? round($stats['totalMessagesReceived'] / $numClients, 2) : 0;
        echo "   Avg Messages/Client (Sent/Received): {$avgSent}/{$avgReceived}\n";
        
        echo "\n";
        $lastStatsTime = $currentTime;
    }
    
    usleep(50000); // 50ms
}

// Cleanup: disconnect all clients
echo "\n═══════════════════════════════════════════════════\n";
echo "🛑 Test duration complete. Cleaning up...\n";

foreach ($clients as $idx => $clientState) {
    try {
        if ($clientState['connected']) {
            $clientState['client']->disconnect();
        }
    } catch (Exception $e) {
        // Ignore cleanup errors
    }
}

// Final report
echo "\n╔════════════════════════════════════════════════╗\n";
echo "║              FINAL TEST REPORT                 ║\n";
echo "╚════════════════════════════════════════════════╝\n\n";

$totalDuration = microtime(true) - $startTime;
echo "📈 Test Results:\n";
echo "   Total Duration: " . round($totalDuration, 2) . "s\n";
echo "   Clients Initialized: {$numClients}\n";
echo "   Peak Connected: {$stats['connected']}\n";
echo "   Total Messages Sent: {$stats['totalMessagesSent']}\n";
echo "   Total Messages Received: {$stats['totalMessagesReceived']}\n";
echo "   Total Errors: {$stats['errors']}\n";

if ($stats['totalMessagesSent'] > 0) {
    $msgPerSec = round($stats['totalMessagesSent'] / $totalDuration, 2);
    echo "   Messages/Second: {$msgPerSec}\n";
}

// Per-client final stats
echo "\n📊 Per-Client Stats:\n";
foreach ($clients as $idx => $clientState) {
    $userData = $clientState['userData'];
    $duration = $clientState['connectTime'] ? microtime(true) - $clientState['connectTime'] : 0;
    echo "   [{$idx}] {$userData['username']}\n";
    echo "       - Sent: {$clientState['messagesSent']}\n";
    echo "       - Received: {$clientState['messagesReceived']}\n";
    if ($duration > 0) {
        echo "       - Connected: " . round($duration, 2) . "s\n";
    }
}

echo "\n✅ Test complete!\n";
