<?php
/**
 * WirePhoenix Chat Client
 * 
 * Usage: php chat_client.php [host] [port] [username]
 * Example: php chat_client.php 127.0.0.1 8080
 */

require_once __DIR__ . '/../vendor/autoload.php';
require_once __DIR__ . '/RandomUserGenerator.php';

use PhoenixWire\AsyncClient;
use PhoenixWire\ClientOptions;

$host = $GLOBALS['argv'][1] ?? '127.0.0.1';
$port = (int)($GLOBALS['argv'][2] ?? 8080);
$customUsername = $GLOBALS['argv'][3] ?? null;

// Generate random user data
$userData = RandomUserGenerator::generate();
if ($customUsername) {
    $userData['username'] = $customUsername;
}

echo "╔════════════════════════════════════════╗\n";
echo "║     PhoenixWire Chat Client Demo       ║\n";
echo "║     Version 1.0                        ║\n";
echo "╚════════════════════════════════════════╝\n\n";

echo "👤 Your Profile:\n";
echo "   Username: {$userData['username']}\n";
echo "   Full Name: {$userData['fullName']}\n";
echo "   User ID: {$userData['id']}\n";
echo "   Avatar: {$userData['avatarColor']}\n";
echo "   Status: {$userData['status']}\n\n";

echo "🔗 Connecting to {$host}:{$port}...\n";

try {
    // Create client with auto-reconnect
    $options = new ClientOptions([
        'host' => $host,
        'port' => $port,
        'autoReconnect' => true,
        'token' => $userData['id']
    ]);
    
    $client = new AsyncClient($options);
    
    $messagesSent = 0;
    $messagesReceived = 0;
    $connectedTime = null;
    $lastMessageTime = microtime(true);
    $nextMessageTime = microtime(true) + RandomUserGenerator::getRandomMessageFrequency();
    
    // Connection handlers
    $client->onConnect(function () use (&$client, $userData, &$connectedTime) {
        $connectedTime = microtime(true);
        echo "\n✅ Connected to server!\n";
        echo "   Ready to send/receive messages.\n\n";
        
        // Send join message with user data
        $joinMsg = json_encode([
            'type' => 'USER_JOIN',
            'user' => $userData
        ]);
        $client->send($joinMsg);
    });
    
    // Message handler
    $client->onMessage(function ($payload) use (&$messagesReceived, $userData) {
        $messagesReceived++;
        
        $decodedMsg = @json_decode($payload, true);
        if ($decodedMsg === null) {
            echo "\n📨 Message: {$payload}\n";
        } else {
            switch ($decodedMsg['type'] ?? null) {
                case 'USER_JOINED':
                    $joinedUser = $decodedMsg['user'] ?? [];
                    if ($joinedUser['id'] !== $userData['id']) {
                        echo "\n[✓] {$joinedUser['username']} joined the chat!\n";
                        echo "    Active users: {$decodedMsg['activeUsers']}\n";
                    }
                    break;
                    
                case 'USER_LEFT':
                    $leftUser = $decodedMsg['user'] ?? [];
                    echo "\n[✗] {$leftUser['username']} left the chat.\n";
                    echo "    Active users: {$decodedMsg['activeUsers']}\n";
                    break;
                    
                case 'MESSAGE':
                    $msgUser = $decodedMsg['user'] ?? [];
                    if ($msgUser['id'] !== $userData['id']) {
                        echo "\n💬 [{$msgUser['username']}]: {$decodedMsg['message']}\n";
                    }
                    break;
                    
                default:
                    echo "\n📨 {$payload}\n";
            }
        }
        echo "💬 > ";
    });
    
    // Error handler
    $client->onError(function ($error) {
        echo "\n⚠️  Error: {$error}\n";
    });
    
    // Close handler
    $client->onClose(function () use (&$connectedTime, &$messagesSent, &$messagesReceived) {
        echo "\n\n═══════════════════════════════════════\n";
        echo "🔌 Disconnected from server.\n";
        if ($connectedTime) {
            $duration = microtime(true) - $connectedTime;
            echo "Session duration: " . round($duration, 2) . "s\n";
        }
        echo "Messages sent: {$messagesSent}\n";
        echo "Messages received: {$messagesReceived}\n";
        echo "═══════════════════════════════════════\n";
    });
    
    echo "💬 > ";
    
    // Main event loop
    while (true) {
        // Process server messages
        $client->tick();
        
        // Auto-send messages at random intervals
        if (microtime(true) >= $nextMessageTime && $client->isConnected()) {
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
            $messagesSent++;
            
            echo "\n[You]: {$chatMessage}\n";
            echo "💬 > ";
            
            // Set next message time
            $nextMessageTime = microtime(true) + RandomUserGenerator::getRandomMessageFrequency();
        }
        
        usleep(50000); // 50ms
    }
    
} catch (Exception $e) {
    echo "\n❌ Error: {$e->getMessage()}\n";
    echo "Connection failed. Make sure the server is running.\n";
    exit(1);
}
