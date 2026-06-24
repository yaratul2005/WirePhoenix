<?php
namespace PhoenixWire;

class Server {
    /** @var callable */
    private $onConnect;
    /** @var callable */
    private $onMessage;
    /** @var callable */
    private $onClose;

    private string $host;
    private int $port;

    public function __construct(string $host, int $port) {
        $this->host = $host;
        $this->port = $port;
    }

    public function onConnect(callable $callback): self {
        $this->onConnect = $callback;
        return $this;
    }

    public function onMessage(callable $callback): self {
        $this->onMessage = $callback;
        return $this;
    }

    public function onClose(callable $callback): self {
        $this->onClose = $callback;
        return $this;
    }

    public function listen(): void {
        // Here we would bind the native server socket and enter the accept loop.
        // For each accepted client, we trigger onConnect.
        // As data flows in, we construct PhoenixWire\Message and trigger onMessage.
    }
}
