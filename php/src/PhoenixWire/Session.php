<?php
namespace PhoenixWire;

/**
 * Represents an active server-side connection.
 */
class Session {
    private string $id;
    private $stream; // Raw socket or wrapped native object

    public function __construct($stream, string $id = null) {
        $this->stream = $stream;
        $this->id = $id ?? bin2hex(random_bytes(8));
    }

    public function id(): string {
        return $this->id;
    }

    public function send(Message|string $data): void {
        // Pseudo-implementation mapping to underlying stream
        // $payload = is_string($data) ? $data : $data->payload;
        // fwrite($this->stream, $payload);
    }

    public function close(): void {
        // fclose($this->stream);
    }

    public function stats(): array {
        return [
            'id' => $this->id,
            'state' => 'READY', // Mocked
        ];
    }
}
