<?php
namespace PhoenixWire;

/**
 * Event-driven / callback-based wrapper for asynchronous usage.
 */
class AsyncClient extends Client {
    /** @var callable|null */
    private $onConnect = null;
    /** @var callable|null */
    private $onMessage = null;
    /** @var callable|null */
    private $onClose = null;
    /** @var callable|null */
    private $onError = null;

    private int $reconnectAttempts = 0;
    private bool $wasConnected = false;
    private float $lastTickTime = 0.0;

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
     * Ticks the internal state machine. In a true async framework (like Revolt, Amp, OpenSwoole),
     * this logic would be driven by stream polling (e.g. stream_select or epoll).
     */
    public function tick(): void {
        $state = $this->getState();
        $now = microtime(true);

        // Handle initial connection or reconnect logic here if we wanted non-blocking auto-reconnect

        if ($state === \PhoenixWire\Client::STATE_READY) {
            if (!$this->wasConnected) {
                $this->wasConnected = true;
                $this->reconnectAttempts = 0;
                if ($this->onConnect) {
                    ($this->onConnect)($this);
                }
            }

            // Poll for messages
            // $messages = $this->nativeClient->readAvailable();
            // foreach ($messages as $msg) {
            //     if ($this->onMessage) {
            //         ($this->onMessage)($this, $msg);
            //     }
            // }

        } elseif ($state === \PhoenixWire\Client::STATE_CLOSED || $state === \PhoenixWire\Client::STATE_DISCONNECTED) {
            if ($this->wasConnected) {
                $this->wasConnected = false;
                if ($this->onClose) {
                    ($this->onClose)($this);
                }

                // Trigger auto-reconnect if enabled
                // ...
            }
        }

        $this->lastTickTime = $now;
    }

    public function triggerError(\Throwable $e): void {
        if ($this->onError) {
            ($this->onError)($this, $e);
        } else {
            throw $e;
        }
    }
}
