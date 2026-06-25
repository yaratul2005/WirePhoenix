<?php
namespace PhoenixWire;

/**
 * Represents an active server-side connection.
 */
class Session {
    private string $id;
    private $stream; // Raw socket or wrapped native object

    private Stats $stats;
    private int $lastActivityMs;

    public function __construct($stream, string $id = null) {
        $this->stream = $stream;
        $this->id = $id ?? bin2hex(random_bytes(8));
        $this->stats = new Stats();
        $this->lastActivityMs = (int)(microtime(true) * 1000);
        $this->stats->connectedSince = microtime(true);
    }

    public function id(): string {
        return $this->id;
    }

    public function send(Message|string $data): void {
        $payload = is_string($data) ? $data : $data->payload;

        $this->stats->messagesSent++;
        $this->stats->bytesSent += strlen($payload);
        $this->lastActivityMs = (int)(microtime(true) * 1000);

        // Native send logic
    }

    public function close(): void {
        // Native close logic
    }

    public function stats(): array {
        $data = $this->stats->toArray();
        $data['id'] = $this->id;
        $data['lastActivityMs'] = $this->lastActivityMs;
        return $data;
    }

    public function recordReceive(int $bytes): void {
        $this->stats->messagesReceived++;
        $this->stats->bytesReceived += $bytes;
        $this->lastActivityMs = (int)(microtime(true) * 1000);
    }
}
