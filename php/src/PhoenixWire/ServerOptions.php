<?php
namespace PhoenixWire;

class ServerOptions {
    public function __construct(
        public readonly int $maxSessions = 10000,
        public readonly int $handshakeTimeoutMs = 3000,
        public readonly int $authTimeoutMs = 3000,
        public readonly int $idleTimeoutMs = 300000, // 5 mins idle disconnect
        public readonly int $maxInflightBytesPerSession = 5242880, // 5MB backlog cap
        public readonly bool $requireAuth = true,
        public readonly bool $acceptResume = true
    ) {}

    public static function default(): self {
        return new self();
    }
}
