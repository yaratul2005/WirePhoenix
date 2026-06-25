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
    private float $nextReconnectTime = 0.0;

    private Stats $stats;
    private ClientOptions $options;

    public function __construct(array|ClientOptions $options = []) {
        parent::__construct($options);
        // We re-fetch options locally so we can access policy logic
        $this->options = is_array($options) ? new ClientOptions($options) : $options;
        $this->stats = new Stats();
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

    public function onError(callable $callback): self {
        $this->onError = $callback;
        return $this;
    }

    public function getStats(): Stats {
        return $this->stats;
    }

    private function calculateBackoff(): int {
        $policy = $this->options->policy;
        $delay = $policy->initialBackoffMs * pow($policy->backoffMultiplier, $this->reconnectAttempts);
        return min((int)$delay, $policy->maxBackoffMs);
    }

    /**
     * Ticks the internal state machine.
     */
    public function tick(): void {
        $state = $this->getState();
        $now = microtime(true);

        if ($state === \PhoenixWire\Client::STATE_CLOSED || $state === \PhoenixWire\Client::STATE_DISCONNECTED) {
            if ($this->wasConnected) {
                $this->wasConnected = false;
                $this->stats->connectedSince = 0.0;
                if ($this->onClose) {
                    ($this->onClose)($this);
                }
            }

            // Auto-reconnect logic
            if ($this->options->autoReconnect) {
                if ($this->reconnectAttempts < $this->options->policy->maxReconnectAttempts) {
                    if ($this->nextReconnectTime === 0.0) {
                        $backoffMs = $this->calculateBackoff();
                        $this->nextReconnectTime = $now + ($backoffMs / 1000.0);
                    } elseif ($now >= $this->nextReconnectTime) {
                        $this->reconnectAttempts++;
                        $this->stats->reconnectCount++;
                        $this->nextReconnectTime = 0.0;

                        try {
                            // Extract host/port from some internal tracking if we stored it
                            // For simplicity, assuming a cached internal state or requiring user to pass them.
                            // In a real framework, AsyncClient knows its configured DSN.
                            // $this->connect($this->host, $this->port);
                        } catch (\Throwable $e) {
                            $this->triggerError(new ConnectionException("Reconnect failed", 0, $e));
                        }
                    }
                }
            }

        } elseif ($state === \PhoenixWire\Client::STATE_READY) {
            if (!$this->wasConnected) {
                $this->wasConnected = true;
                $this->reconnectAttempts = 0;
                $this->nextReconnectTime = 0.0;
                $this->stats->connectedSince = $now;

                if ($this->onConnect) {
                    ($this->onConnect)($this);
                }
            }

            // In native extension, we'd poll readQueue here
            // e.g.: while ($msg = $this->nativeClient->recv()) {
            //     $this->stats->messagesReceived++;
            //     $this->stats->bytesReceived += strlen($msg->payload);
            //     if ($this->onMessage) ($this->onMessage)($this, $msg);
            // }
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
