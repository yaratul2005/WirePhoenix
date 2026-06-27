<?php
/**
 * WirePhoenix Chat Server
 * 
 * Usage: php chat_server.php [host] [port]
 * Example: php chat_server.php 127.0.0.1 8080
 */

require_once __DIR__ . '/../vendor/autoload.php';
require_once __DIR__ . '/RandomUserGenerator.php';

use PhoenixWire\Server;
use PhoenixWire\ServerOptions;

$host = $GLOBALS['argv'][1] ?? '127.0.0.1';
$port = (int)($GLOBALS['argv'][2] ?? 8080);

// Store connected sessions with their user data
$sessions = [];
$sessionCount = 0;

echo "╔════════════════════════════════════════╗\n";
echo "║     PhoenixWire Chat Server Demo       ║\n";
echo "║     Version 1.0                        ║\n";
echo "╚════════════════════════════════════════╝\n\n";

echo "🚀 Starting server on {$host}:{$port}...\n\n";

$server = new Server($host, $port);

// Handle new connections
$server->onConnect(function ($session) use (&$sessions, &$sessionCount) {
    $sessionId = $session->id();
    $userData = RandomUserGenerator::generate();
    
    $sessions[$sessionId] = $userData;
    $sessionCount++;
    
    echo "[✓] New user connected! ({$sessionCount} active)\n";
    echo "   User ID: {$userData['id']}\n";
    echo "   Username: {$userData['username']}\n";
    echo "   Full Name: {$userData['fullName']}\n";
    echo "   Session: {$sessionId}\n";
    echo "   Avatar Color: {$userData['avatarColor']}\n\n";
    
    // Notify all users about the join
    $welcomeMsg = json_encode([
        'type' => 'USER_JOINED',
        'user' => $userData,
        'timestamp' => microtime(true),
        'activeUsers' => count($sessions)
    ]);
    
    // Broadcast join message to all
    foreach ($sessions as $sid => $sdata) {
        if ($sid !== $sessionId) {
            try {
                $s = $session; // This is the new session
                $s->send($welcomeMsg);
            } catch (Exception $e) {
                // Session may have closed
            }
        }
    }
});

// Handle incoming messages
$server->onMessage(function ($session, $message) use (&$sessions) {
    $sessionId = $session->id();
    
    if (!isset($sessions[$sessionId])) {
        return;
    }
    
    $user = $sessions[$sessionId];
    $user['messageCount']++;
    $sessions[$sessionId] = $user;
    
    $payload = $message->getPayload();
    
    // Parse incoming message (could be JSON or plain text)
    $decodedMsg = @json_decode($payload, true);
    if ($decodedMsg === null) {
        $decodedMsg = ['text' => $payload];
    }
    
    echo "[💬] Message from {$user['username']}: {$decodedMsg['text']}\n";
    
    // Create broadcast message
    $broadcastMsg = json_encode([
        'type' => 'MESSAGE',
        'user' => [
            'id' => $user['id'],
            'username' => $user['username'],
            'fullName' => $user['fullName'],
            'avatarColor' => $user['avatarColor']
        ],
        'message' => $decodedMsg['text'] ?? $payload,
        'timestamp' => microtime(true)
    ]);
    
    // Broadcast to all connected users
    foreach ($sessions as $sid => $sdata) {
        try {
            if ($sid !== $sessionId) {
                // In a real scenario, you'd have a reference to each session
                // For this demo, we'll just log the broadcast
            }
        } catch (Exception $e) {
            // Session may have closed
        }
    }
});

// Handle client disconnect
$server->onClose(function ($session) use (&$sessions, &$sessionCount) {
    $sessionId = $session->id();
    
    if (isset($sessions[$sessionId])) {
        $user = $sessions[$sessionId];
        unset($sessions[$sessionId]);
        $sessionCount--;
        
        echo "[✗] User disconnected: {$user['username']}\n";
        echo "   Messages sent: {$user['messageCount']}\n";
        echo "   Session duration: " . round(microtime(true) - $user['joinedAt'], 2) . "s\n";
        echo "   Active users: {$sessionCount}\n\n";
        
        // Notify remaining users
        $leaveMsg = json_encode([
            'type' => 'USER_LEFT',
            'user' => $user,
            'timestamp' => microtime(true),
            'activeUsers' => $sessionCount
        ]);
    }
});

// Handle errors
$server->onError(function ($session, $error) use (&$sessions) {
    $sessionId = $session->id();
    if (isset($sessions[$sessionId])) {
        $user = $sessions[$sessionId];
        echo "[⚠] Error for {$user['username']}: {$error}\n";
    } else {
        echo "[⚠] Error: {$error}\n";
    }
});

// Main server loop
echo "✨ Server ready. Waiting for connections...\n";
echo "═══════════════════════════════════════════\n\n";

$lastStatsPrint = microtime(true);

try {
    while (true) {
        // Try to accept new connections
        try {
            $session = $server->accept(100); // 100ms timeout
            if ($session) {
                // Process connection
            }
        } catch (Exception $e) {
            // No connection or error
        }
        
        // Process existing sessions (this would be part of the server loop)
        // For now, just keep the loop alive
        
        // Print stats every 30 seconds
        if (microtime(true) - $lastStatsPrint > 30) {
            $timestamp = date('Y-m-d H:i:s');
            echo "[\$timestamp] Server Stats: {$sessionCount} active users\n";
            $lastStatsPrint = microtime(true);
        }
        
        usleep(100000); // 100ms sleep
    }
} catch (Exception $e) {
    echo "\n\n❌ Server error: {$e->getMessage()}\n";
    echo "Shutting down gracefully...\n";
}

echo "\n✅ Server shutdown complete.\n";
