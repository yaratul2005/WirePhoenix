<?php
namespace PhoenixWire;

/**
 * Event-driven / callback-based wrapper for asynchronous usage.
 */
class AsyncClient extends Client {
    /** @var callable */
    private $onConnect;
    /** @var callable */
    private $onMessage;
    /** @var callable */
    private $onClose;
    /** @var callable */
    private $onError;

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

    public function onError(callable $callback): self {
        $this->onError = $callback;
        return $this;
    }

    /**
     * In a real event loop (e.g. Swoole/ReactPHP), this would register
     * the socket file descriptor for non-blocking read/write events.
     *
     * For a simple callback runtime loop:
     */
    public function tick(): void {
        // Here we'd poll the native client state.
        // If data is available, trigger $this->onMessage($msg).
        // If disconnected natively, trigger $this->onClose().
        // If connected natively, trigger $this->onConnect().
    }
}
