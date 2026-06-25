<?php
namespace PhoenixWire;

class Server {
    /** @var callable|null */
    private $onConnect = null;
    /** @var callable|null */
    private $onMessage = null;
    /** @var callable|null */
    private $onClose = null;
    /** @var callable|null */
    private $onError = null;

    private string $host;
    private int $port;
    private ServerOptions $options;

    /** @var array<string, Session> */
    private array $sessions = [];

    public function __construct(string $host, int $port, ?ServerOptions $options = null) {
        $this->host = $host;
        $this->port = $port;
        $this->options = $options ?? ServerOptions::default();
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

    /**
     * Broadcasts a message to all connected sessions.
     * Optionally filter by a callback.
     */
    public function broadcast(Message|string $message, ?callable $filter = null): void {
        foreach ($this->sessions as $session) {
            if ($filter === null || $filter($session)) {
                $session->send($message);
            }
        }
    }

    /**
     * Registers a session, applying admission control.
     */
    public function addSession(Session $session): bool {
        if (count($this->sessions) >= $this->options->maxSessions) {
            $session->close(); // Admission rejected
            return false;
        }

        $this->sessions[$session->id()] = $session;
        if ($this->onConnect) {
            try {
                ($this->onConnect)($session);
            } catch (\Throwable $e) {
                if ($this->onError) {
                    ($this->onError)($e, $session);
                }
            }
        }
        return true;
    }

    /**
     * Removes a session.
     */
    public function removeSession(string $id): void {
        if (isset($this->sessions[$id])) {
            $session = $this->sessions[$id];
            unset($this->sessions[$id]);
            if ($this->onClose) {
                try {
                    ($this->onClose)($session);
                } catch (\Throwable $e) {
                    if ($this->onError) {
                        ($this->onError)($e, $session);
                    }
                }
            }
        }
    }

    /**
     * Runs idle eviction sweep.
     */
    public function sweepIdleSessions(int $nowMs): void {
        foreach ($this->sessions as $id => $session) {
            $stats = $session->stats();
            if (($nowMs - $stats['lastActivityMs']) > $this->options->idleTimeoutMs) {
                $session->close();
                $this->removeSession($id);
            }
        }
    }

    /**
     * Listens for incoming connections.
     */
    public function listen(): void {
        // Native listen loop implementation goes here
    }
}
