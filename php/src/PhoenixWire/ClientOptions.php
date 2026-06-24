<?php
namespace PhoenixWire;

class ClientOptions {
    public int $timeoutMs = 5000;
    public bool $autoReconnect = true;
    public int $maxReconnectAttempts = 5;
    public bool $enableResume = true;
    public int $heartbeatIntervalMs = 30000;
    public int $maxFrameSize = 1048576; // 1MB
    public ?string $bearerToken = null;

    public function __construct(array $options = []) {
        foreach ($options as $key => $value) {
            if (property_exists($this, $key)) {
                $this->$key = $value;
            }
        }
    }
}
