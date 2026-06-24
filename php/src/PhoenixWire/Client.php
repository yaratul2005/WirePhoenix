<?php
namespace PhoenixWire;

/**
 * PHP userland wrapper around the native \PhoenixWire\Client extension class.
 */
class Client {
    private \PhoenixWire\Client $nativeClient;
    private ClientOptions $options;

    public function __construct(array|ClientOptions $options = []) {
        if (is_array($options)) {
            $this->options = new ClientOptions($options);
        } else {
            $this->options = $options;
        }

        // This instantiates the native extension object
        $this->nativeClient = new \PhoenixWire\Client();
    }

    public function connect(string $host, int $port): bool {
        // Here we will eventually pass down options (like tokens) to the native layer
        // For now, call the native connect
        return $this->nativeClient->connect($host, $port);
    }

    public function disconnect(): bool {
        return $this->nativeClient->disconnect();
    }

    public function getState(): int {
        return $this->nativeClient->getState();
    }

    // Abstracting out read/write for the native client will happen here
}
