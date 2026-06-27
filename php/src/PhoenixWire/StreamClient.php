<?php
namespace PhoenixWire;

use Exception;

/**
 * Pure-PHP stream socket implementation of the PhoenixWire protocol.
 * Used as a fallback in environments (like shared hosting) where the native extension is unavailable.
 */
class StreamClient {
    private $stream = null;
    private ClientOptions $options;
    private int $state = 0; // PW_STATE_DISCONNECTED

    // Protocol constants
    const PW_MAGIC_BYTE_1 = 0x50; // 'P'
    const PW_MAGIC_BYTE_2 = 0x57; // 'W'
    const PW_VERSION_1 = 1;

    // Opcodes
    const PW_OPCODE_DATA_BINARY = 0x01;
    const PW_OPCODE_DATA_TEXT   = 0x02;
    const PW_OPCODE_PING        = 0x09;
    const PW_OPCODE_PONG        = 0x0A;
    const PW_OPCODE_CLOSE       = 0x08;
    const PW_OPCODE_AUTH        = 0x0F;
    const PW_OPCODE_AUTH_OK     = 0x10;

    // Flags
    const PW_FLAG_FIN = 0x80;

    // Capabilities
    const PW_CAP_AUTH_BEARER = 0x04;

    public function __construct(array|ClientOptions $options = []) {
        if (is_array($options)) {
            $this->options = new ClientOptions($options);
        } else {
            $this->options = $options;
        }
    }

    public function connect(string $host, int $port): bool {
        $scheme = $this->options->useTls ? "tls://" : "tcp://";
        $address = "{$scheme}{$host}:{$port}";

        $context = stream_context_create();
        if ($this->options->useTls) {
            stream_context_set_option($context, 'ssl', 'crypto_method', STREAM_CRYPTO_METHOD_TLSv1_2_CLIENT | STREAM_CRYPTO_METHOD_TLSv1_3_CLIENT);
            stream_context_set_option($context, 'ssl', 'verify_peer', true);
            stream_context_set_option($context, 'ssl', 'verify_peer_name', true);
        }

        $this->stream = stream_socket_client($address, $errno, $errstr, 5.0, STREAM_CLIENT_CONNECT, $context);

        if (!$this->stream) {
            throw new Exception("Connection failed: $errstr ($errno)");
        }

        stream_set_blocking($this->stream, true);

        // Handshake
        $nonce = random_bytes(16);
        $hello = pack("CCN",
            self::PW_MAGIC_BYTE_1,
            self::PW_MAGIC_BYTE_2,
            self::PW_VERSION_1
        );
        $hello .= pack("N", self::PW_CAP_AUTH_BEARER);
        $hello .= $nonce;
        $hello .= pack("C", 0); // no resume token

        fwrite($this->stream, $hello);
        fflush($this->stream);

        $serverHelloRaw = $this->readExact(42);

        $magic1 = ord($serverHelloRaw[0]);
        $magic2 = ord($serverHelloRaw[1]);
        $version = ord($serverHelloRaw[2]);
        $statusRaw = substr($serverHelloRaw, 39, 2);
        $status = unpack("n", $statusRaw)[1];

        if ($magic1 !== self::PW_MAGIC_BYTE_1 || $magic2 !== self::PW_MAGIC_BYTE_2) {
            throw new Exception("Invalid protocol magic bytes");
        }
        if ($status !== 0) {
            throw new Exception("Server rejected handshake, status: " . $status);
        }

        $token = $this->options->bearerToken ?? "secret_token";

        $authPayload = pack("C", 1); // PW_AUTH_METHOD_BEARER = 1
        $authPayload .= pack("N", strlen($token));
        $authPayload .= $token;

        $this->sendFrame(self::PW_OPCODE_AUTH, $authPayload);

        $authRes = $this->readFrame();
        if ($authRes['opcode'] !== self::PW_OPCODE_AUTH_OK) {
            throw new Exception("Authentication failed, got opcode: " . $authRes['opcode']);
        }

        stream_set_blocking($this->stream, false);
        $this->state = 4; // PW_STATE_READY

        return true;
    }

    public function send(string $data): void {
        $this->sendFrame(self::PW_OPCODE_DATA_TEXT, $data);
    }

    public function receive(int $timeoutMs = 0): ?string {
        if (!$this->stream) return null;

        $read = [$this->stream];
        $write = null;
        $except = null;

        $sec = floor($timeoutMs / 1000);
        $usec = ($timeoutMs % 1000) * 1000;

        if (stream_select($read, $write, $except, $sec, $usec) > 0) {
            try {
                $frame = $this->readFrame();
                if ($frame['opcode'] === self::PW_OPCODE_DATA_TEXT || $frame['opcode'] === self::PW_OPCODE_DATA_BINARY) {
                    return $frame['payload'];
                } else if ($frame['opcode'] === self::PW_OPCODE_PING) {
                    $this->sendFrame(self::PW_OPCODE_PONG, "");
                } else if ($frame['opcode'] === self::PW_OPCODE_CLOSE) {
                    $this->disconnect();
                }
            } catch (Exception $e) {
                // Connection closed or broken frame
                $this->disconnect();
            }
        }
        return null;
    }

    public function disconnect(): void {
        if ($this->stream) {
            $this->sendFrame(self::PW_OPCODE_CLOSE, "");
            fclose($this->stream);
            $this->stream = null;
            $this->state = 0;
        }
    }

    private function sendFrame(int $opcode, string $payload): void {
        if (!$this->stream) return;

        $len = strlen($payload);
        $header = pack("CCCCC", self::PW_MAGIC_BYTE_1, self::PW_MAGIC_BYTE_2, self::PW_VERSION_1, self::PW_FLAG_FIN, $opcode);

        $header .= chr(0); // stream ID = 0 varint

        if ($len == 0) {
            $header .= chr(0);
        } else {
            $lenBytes = "";
            $v = $len;
            do {
                $byte = $v & 0x7F;
                $v >>= 7;
                if ($v > 0) $byte |= 0x80;
                $lenBytes .= chr($byte);
            } while ($v > 0);
            $header .= $lenBytes;
        }

        fwrite($this->stream, $header . $payload);
        fflush($this->stream);
    }

    private function readFrame(): array {
        $hdrRaw = $this->readExact(5);
        $hdr = unpack("Cmagic1/Cmagic2/Cversion/Cflags/Copcode", $hdrRaw);

        if ($hdr['magic1'] !== self::PW_MAGIC_BYTE_1 || $hdr['magic2'] !== self::PW_MAGIC_BYTE_2) {
            throw new Exception("Invalid frame magic");
        }

        $streamId = $this->readVarint();
        $length = $this->readVarint();

        $payload = "";
        if ($length > 0) {
            $payload = $this->readExact($length);
        }

        return [
            'opcode' => $hdr['opcode'],
            'payload' => $payload
        ];
    }

    private function readVarint(): int {
        $value = 0;
        $shift = 0;
        while (true) {
            $bRaw = $this->readExact(1);
            $byte = ord($bRaw);
            $value |= ($byte & 0x7F) << $shift;
            if (($byte & 0x80) == 0) break;
            $shift += 7;
        }
        return $value;
    }

    private function readExact(int $length): string {
        $data = "";
        while (strlen($data) < $length) {
            $chunk = fread($this->stream, $length - strlen($data));
            if ($chunk === false || $chunk === "") {
                throw new Exception("Connection closed unexpectedly");
            }
            $data .= $chunk;
        }
        return $data;
    }
}
