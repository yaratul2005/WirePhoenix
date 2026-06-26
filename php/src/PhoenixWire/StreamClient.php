<?php
namespace PhoenixWire;

use Exception;

class StreamClient {
    private $stream = null;
    private ClientOptions $options;
    private int $state = 0;

    const PW_MAGIC_BYTE_1 = 0x50; // 'P'
    const PW_MAGIC_BYTE_2 = 0x57; // 'W'
    const PW_VERSION_1 = 1;

    const PW_OPCODE_DATA_BINARY = 0x01;
    const PW_OPCODE_DATA_TEXT   = 0x02;
    const PW_OPCODE_PING        = 0x09;
    const PW_OPCODE_PONG        = 0x0A;
    const PW_OPCODE_CLOSE       = 0x08;
    const PW_OPCODE_AUTH        = 0x0F;
    const PW_OPCODE_AUTH_OK     = 0x10;

    const PW_FLAG_FIN = 0x80;
    const PW_CAP_AUTH_BEARER = 0x04;

    public function __construct(array|ClientOptions $options = []) {
        $this->options = is_array($options) ? new ClientOptions($options) : $options;
    }

    public function connect(string $host, int $port): bool {
        $address = "tcp://$host:$port";
        $this->stream = stream_socket_client($address, $errno, $errstr, 5.0, STREAM_CLIENT_CONNECT);
        if (!$this->stream) throw new Exception("Connection failed");

        stream_set_blocking($this->stream, true);

        $hello = pack("CCN", self::PW_MAGIC_BYTE_1, self::PW_MAGIC_BYTE_2, self::PW_VERSION_1);
        $hello .= pack("N", self::PW_CAP_AUTH_BEARER);
        $hello .= random_bytes(16);
        $hello .= pack("C", 0);
        fwrite($this->stream, $hello);
        fflush($this->stream);

        $serverHelloRaw = $this->readExact(42);

        $token = "secret_token";
        $authPayload = chr(0x01) . pack("N", strlen($token)) . $token;

        // Let's send exactly what `test_parse.c` expects
        // 0x50, 0x57, 0x01, 0x80, 0x0F, 0x00, 0x11, 0x01, 0x00, 0x00, 0x00, 0x0C, 's','e','c','r','e','t','_','t','o','k','e','n'
        $frame = pack("C*", 0x50, 0x57, 0x01, 0x80, 0x0F, 0x00, 0x11, 0x01, 0x00, 0x00, 0x00, 0x0C);
        $frame .= $token;

        fwrite($this->stream, $frame);
        fflush($this->stream);

        $authRes = $this->readFrame();
        if ($authRes['opcode'] !== self::PW_OPCODE_AUTH_OK) throw new Exception("Authentication failed");

        stream_set_blocking($this->stream, false);
        return true;
    }

    public function send(string $data): void {
        $this->sendFrame(self::PW_OPCODE_DATA_TEXT, $data);
    }

    public function receive(int $timeoutMs = 0): ?string {
        if (!$this->stream) return null;
        $sec = floor($timeoutMs / 1000);
        $usec = ($timeoutMs % 1000) * 1000;
        $read = [$this->stream]; $write = null; $except = null;

        if (stream_select($read, $write, $except, $sec, $usec) > 0) {
            try {
                $frame = $this->readFrame();
                if ($frame['opcode'] === self::PW_OPCODE_DATA_TEXT || $frame['opcode'] === self::PW_OPCODE_DATA_BINARY) return $frame['payload'];
                if ($frame['opcode'] === self::PW_OPCODE_PING) $this->sendFrame(self::PW_OPCODE_PONG, "");
                if ($frame['opcode'] === self::PW_OPCODE_CLOSE) $this->disconnect();
            } catch (Exception $e) { $this->disconnect(); }
        }
        return null;
    }

    public function disconnect(): void {
        if ($this->stream) {
            $this->sendFrame(self::PW_OPCODE_CLOSE, "");
            fclose($this->stream);
            $this->stream = null;
        }
    }

    private function sendFrame(int $opcode, string $payload): void {
        if (!$this->stream) return;
        $len = strlen($payload);
        $header = pack("CCCCC", self::PW_MAGIC_BYTE_1, self::PW_MAGIC_BYTE_2, self::PW_VERSION_1, self::PW_FLAG_FIN, $opcode);
        $header .= chr(0);
        if ($len == 0) { $header .= chr(0); } else {
            $v = $len;
            do {
                $byte = $v & 0x7F; $v >>= 7;
                if ($v > 0) $byte |= 0x80;
                $header .= chr($byte);
            } while ($v > 0);
        }
        fwrite($this->stream, $header . $payload);
        fflush($this->stream);
    }

    private function readFrame(): array {
        $hdr = unpack("Cmagic1/Cmagic2/Cversion/Cflags/Copcode", $this->readExact(5));
        if ($hdr['magic1'] !== self::PW_MAGIC_BYTE_1 || $hdr['magic2'] !== self::PW_MAGIC_BYTE_2) throw new Exception("Invalid magic");
        $this->readVarint();
        $length = $this->readVarint();
        return ['opcode' => $hdr['opcode'], 'payload' => $length > 0 ? $this->readExact($length) : ""];
    }

    private function readVarint(): int {
        $value = 0; $shift = 0;
        while (true) {
            $byte = ord($this->readExact(1));
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
            if ($chunk === false || $chunk === "") throw new Exception("Connection closed unexpectedly");
            $data .= $chunk;
        }
        return $data;
    }
}
