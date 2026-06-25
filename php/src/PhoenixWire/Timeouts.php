<?php
namespace PhoenixWire;

/**
 * Defines timeout policies for PhoenixWire connections.
 */
class Timeouts {
    public function __construct(
        public readonly int $connectTimeoutMs = 5000,
        public readonly int $handshakeTimeoutMs = 2000,
        public readonly int $authTimeoutMs = 2000,
        public readonly int $idleTimeoutMs = 60000,
        public readonly int $readDeadlineMs = 0, // 0 = no absolute deadline
        public readonly int $writeDeadlineMs = 0
    ) {}

    public static function default(): self {
        return new self();
    }

    public static function aggressive(): self {
        return new self(1000, 500, 500, 15000, 500, 500);
    }

    public static function relaxed(): self {
        return new self(15000, 5000, 5000, 120000, 0, 0);
    }
}
