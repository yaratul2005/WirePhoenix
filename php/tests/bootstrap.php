<?php
// Mock Native Client if not loaded
if (!extension_loaded('phoenixwire') && !class_exists('PhoenixWire\\Client')) {
    eval('
    namespace PhoenixWire {
        class Client {
            public const STATE_DISCONNECTED = 0;
            public const STATE_CONNECTING = 1;
            public const STATE_HANDSHAKING = 2;
            public const STATE_AUTHENTICATING = 3;
            public const STATE_READY = 4;
            public const STATE_DRAINING = 5;
            public const STATE_CLOSING = 6;
            public const STATE_CLOSED = 7;

            protected int $nativeState = self::STATE_DISCONNECTED;

            public function __construct($options = []) {}
            public function connect(string $host, int $port): bool { $this->nativeState = self::STATE_CONNECTING; return true; }
            public function disconnect(): bool { $this->nativeState = self::STATE_CLOSED; return true; }
            public function getState(): int { return $this->nativeState; }

            // Testing helper
            public function _setMockState(int $state) { $this->nativeState = $state; }
        }
    }
    ');
}

// Autoloader/bootstrap for testing without compiling native extension
spl_autoload_register(function ($class) {
    if ($class === 'PhoenixWire\\Client') return; // Handled by mock

    $prefix = 'PhoenixWire\\';
    $base_dir = __DIR__ . '/../src/PhoenixWire/';
    $len = strlen($prefix);
    if (strncmp($prefix, $class, $len) !== 0) {
        return;
    }
    $relative_class = substr($class, $len);
    $file = $base_dir . str_replace('\\', '/', $relative_class) . '.php';
    if (file_exists($file)) {
        require_once $file;
    }
});
