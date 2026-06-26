<?php
namespace PhoenixWire;

class ClientOptions {
    public \PhoenixWire\Timeouts $timeouts;
    public bool $autoReconnect = true;
    public int $maxReconnectAttempts = 5;
    public bool $enableResume = true;
    public int $heartbeatIntervalMs = 30000;
    public int $maxFrameSize = 1048576; // 1MB
    public ?string $bearerToken = null;
    public bool $useTls = false;

    public Policy $policy;

    public function __construct(array $options = []) {
        $this->policy = Policy::default();
        $this->timeouts = Timeouts::default();
        foreach ($options as $key => $value) {
            if (property_exists($this, $key)) {
                $this->$key = $value;
            }
        }

        // Handle snake_case alternatives for usability
        if (isset($options['auth_token']) && $this->bearerToken === null) {
            $this->bearerToken = $options['auth_token'];
        }
        if (isset($options['use_tls'])) {
            $this->useTls = (bool)$options['use_tls'];
        }
    }
}
