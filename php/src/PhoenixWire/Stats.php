<?php
namespace PhoenixWire;

/**
 * Encapsulates runtime observability metrics.
 */
class Stats {
    public int $messagesSent = 0;
    public int $messagesReceived = 0;
    public int $bytesSent = 0;
    public int $bytesReceived = 0;
    public int $reconnectCount = 0;
    public int $droppedMessages = 0;
    public int $backlogDepthBytes = 0;

    public int $lastPingLatencyMs = 0;
    public float $connectedSince = 0.0;

    public function toArray(): array {
        return [
            'messagesSent' => $this->messagesSent,
            'messagesReceived' => $this->messagesReceived,
            'bytesSent' => $this->bytesSent,
            'bytesReceived' => $this->bytesReceived,
            'reconnectCount' => $this->reconnectCount,
            'droppedMessages' => $this->droppedMessages,
            'backlogDepthBytes' => $this->backlogDepthBytes,
            'lastPingLatencyMs' => $this->lastPingLatencyMs,
            'uptimeSeconds' => $this->connectedSince > 0 ? (microtime(true) - $this->connectedSince) : 0,
        ];
    }
}
