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

    /** @var array<string, Session> */
    private array $sessions = [];

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

    public function onError(callable $callback): self {
        $this->onError = $callback;
        return $this;
    }

    /**
     * Broadcasts a message to all connected sessions.
     */
    public function broadcast(Message|string $message): void {
        foreach ($this->sessions as $session) {
            $session->send($message);
        }
    }

    /**
     * Registers a session.
     */
    public function addSession(Session $session): void {
        $this->sessions[$session->id()] = $session;
        if ($this->onConnect) {
            ($this->onConnect)($session);
        }
    }

    /**
     * Removes a session.
     */
    public function removeSession(string $id): void {
        if (isset($this->sessions[$id])) {
            $session = $this->sessions[$id];
            unset($this->sessions[$id]);
            if ($this->onClose) {
                ($this->onClose)($session);
            }
        }
    }

    /**
     * Listens for incoming connections.
     * In a native PHP context, this would wrap stream_socket_server and stream_select,
     * or utilize a framework like Swoole/ReactPHP.
     */
    public function listen(): void {
        // Pseudo-implementation mapping
        // $socket = stream_socket_server("tcp://{$this->host}:{$this->port}", $errno, $errstr);
        // while (true) {
        //    $client = stream_socket_accept($socket, -1);
        //    $session = new Session($client);
        //    $this->addSession($session);
        //    // handle read/write loops
        // }
    }
}
