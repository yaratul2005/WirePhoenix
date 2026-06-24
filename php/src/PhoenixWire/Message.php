<?php
namespace PhoenixWire;

class Message {
    public const TYPE_BINARY = 1;
    public const TYPE_TEXT = 2;

    public function __construct(
        public readonly int $type,
        public readonly string $payload,
        public readonly int $streamId = 0
    ) {}
}
