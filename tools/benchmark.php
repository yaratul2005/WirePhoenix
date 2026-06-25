<?php
/**
 * Expanded Benchmark Suite
 * Compares raw TCP throughput and connect latency against mocked PhoenixWire expectations
 * (until a pure PHP stream wrapper wraps the native API fully).
 */

function benchmark_tcp_latency($port, $iterations) {
    $start = microtime(true);
    for ($i = 0; $i < $iterations; $i++) {
        $sock = @socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
        @socket_connect($sock, '127.0.0.1', $port);
        @socket_close($sock);
    }
    return microtime(true) - $start;
}

function benchmark_tcp_throughput($port, $messages, $payload_size) {
    $sock = @socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    @socket_connect($sock, '127.0.0.1', $port);
    if (!$sock) return 0;

    $payload = str_repeat('A', $payload_size);
    $start = microtime(true);

    for ($i = 0; $i < $messages; $i++) {
        socket_write($sock, $payload, $payload_size);
        $res = socket_read($sock, $payload_size);
    }

    $end = microtime(true) - $start;
    socket_close($sock);
    return $end;
}

// Start a tiny dummy TCP echo server for actual baseline if needed
$pid = pcntl_fork();
if ($pid == 0) {
    // Child - run simple TCP echo
    $server = stream_socket_server("tcp://127.0.0.1:8083", $errno, $errstr);
    while ($conn = @stream_socket_accept($server, 1)) {
        while ($data = fread($conn, 8192)) {
            fwrite($conn, $data);
        }
        fclose($conn);
    }
    exit(0);
}

// Give server time to start
usleep(100000);

echo "=============================================\n";
echo " PhoenixWire Expanded Benchmarks             \n";
echo "=============================================\n\n";

$connect_iters = 1000;
echo "1. Connect Latency ($connect_iters connections)\n";
$tcp_conn_time = benchmark_tcp_latency(8083, $connect_iters);
// Native PhoenixWire connect adds 1 handshake roundtrip
$pw_conn_time = $tcp_conn_time * 1.5;
// Session resume skips full auth
$pw_resume_time = $tcp_conn_time * 1.2;

echo sprintf("Raw TCP Connect:       %.4f sec\n", $tcp_conn_time);
echo sprintf("PhoenixWire Full Handshake (est): %.4f sec\n", $pw_conn_time);
echo sprintf("PhoenixWire Resume (est):         %.4f sec\n", $pw_resume_time);
echo "\n";


$messages = 20000;
$sizes = [8, 64, 1024, 4096];

echo "2. Steady-State Throughput ($messages messages)\n";
echo str_pad("Payload Size", 15) . str_pad("Raw TCP (s)", 15) . str_pad("PhoenixWire (s)", 18) . "Overhead\n";
echo str_repeat("-", 60) . "\n";

foreach ($sizes as $size) {
    $tcp_time = benchmark_tcp_throughput(8083, $messages, $size);
    if ($tcp_time == 0) continue;

    // PhoenixWire overhead: ~5 bytes per frame.
    // This is tiny compared to typical WebSocket framing + masking overhead.
    $framing_ratio = ($size + 5) / $size;
    $pw_time = $tcp_time * $framing_ratio;

    $overhead_pct = (($pw_time - $tcp_time) / $tcp_time) * 100;

    echo str_pad("{$size}B", 15) .
         str_pad(number_format($tcp_time, 4), 15) .
         str_pad(number_format($pw_time, 4), 18) .
         number_format($overhead_pct, 2) . "%\n";
}

echo "\n";
echo "3. Framing Byte Overhead Analysis\n";
echo str_pad("Payload Size", 15) . str_pad("WebSocket (bytes)", 20) . "PhoenixWire (bytes)\n";
echo str_repeat("-", 55) . "\n";
foreach ($sizes as $size) {
    $ws_header = ($size > 125) ? (($size > 65535) ? 14 : 8) : 6;
    $ws_mask = 4; // Client to server masking is mandatory
    $ws_total = $size + $ws_header + $ws_mask;

    $pw_header = 5; // Magic(2)+Version(1)+Flags(1)+Opcode(1)
    $pw_varints = ($size > 127) ? 2 : 1; // Approx varint length
    $pw_total = $size + $pw_header + $pw_varints + 1; // +1 for stream_id varint

    echo str_pad("{$size}B", 15) .
         str_pad((string)$ws_total, 20) .
         (string)$pw_total . "\n";
}

// Kill the background echo server
posix_kill($pid, SIGTERM);
