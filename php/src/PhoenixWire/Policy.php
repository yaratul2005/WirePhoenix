<?php
namespace PhoenixWire;

/**
 * Defines resilience and backpressure policies for PhoenixWire clients and servers.
 */
class Policy {
    public function __construct(
        public readonly int $maxReconnectAttempts = 5,
        public readonly int $initialBackoffMs = 1000,
        public readonly int $maxBackoffMs = 30000,
        public readonly float $backoffMultiplier = 1.5,
        public readonly int $maxInflightBytes = 1048576, // 1MB unacknowledged limit
        public readonly int $maxMessageSize = 524288,    // 500KB max individual message
        public readonly int $maxConcurrentStreams = 100
    ) {}

    public static function default(): self {
        return new self();
    }
}
