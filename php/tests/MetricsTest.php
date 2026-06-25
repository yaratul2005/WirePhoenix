<?php
require_once __DIR__ . '/bootstrap.php';

use PhoenixWire\Session;
use PhoenixWire\Stats;
use PhoenixWire\Message;

class MetricsTest {

    public function testSessionStatsMonotonicity() {
        // Create mock stream
        $mockStream = fopen('php://memory', 'w+');
        $session = new Session($mockStream);

        $stats = $session->stats();
        if ($stats['messagesSent'] !== 0) throw new \Exception("Initial messagesSent must be 0");
        if ($stats['bytesSent'] !== 0) throw new \Exception("Initial bytesSent must be 0");

        $msg = new Message(Message::TYPE_TEXT, "Hello");
        $session->send($msg);

        $stats = $session->stats();
        if ($stats['messagesSent'] !== 1) throw new \Exception("messagesSent not updated");
        if ($stats['bytesSent'] !== 5) throw new \Exception("bytesSent not updated (expected 5, got " . $stats['bytesSent'] . ")");

        // Simulating receive
        $session->recordReceive(10);

        $stats = $session->stats();
        if ($stats['messagesReceived'] !== 1) throw new \Exception("messagesReceived not updated");
        if ($stats['bytesReceived'] !== 10) throw new \Exception("bytesReceived not updated");

        // Assert Uptime tracking
        usleep(10000); // 10ms
        $stats2 = $session->stats();
        if ($stats2['uptimeSeconds'] <= 0) throw new \Exception("Uptime not tracking");

        fclose($mockStream);
    }
}
