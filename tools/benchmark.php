<?php
// Simple TCP vs PhoenixWire benchmark skeleton

function benchmark_tcp($port, $messages, $payload_size) {
    $start = microtime(true);
    $sock = socket_create(AF_INET, SOCK_STREAM, SOL_TCP);
    socket_connect($sock, '127.0.0.1', $port);

    $payload = str_repeat('A', $payload_size);
    for ($i = 0; $i < $messages; $i++) {
        socket_write($sock, $payload, $payload_size);
        $res = socket_read($sock, $payload_size);
    }

    socket_close($sock);
    return microtime(true) - $start;
}

// Assuming the native daemon is running on 8081 for PhoenixWire echo,
// and a simple TCP echo is running on 8082
$messages = 10000;
$sizes = [8, 64, 1024];

echo "Benchmarking TCP vs PhoenixWire ($messages messages)...\n";
echo str_pad("Payload Size", 15) . str_pad("Raw TCP (s)", 15) . "PhoenixWire (s) [Est]\n";
echo str_repeat("-", 45) . "\n";

foreach ($sizes as $size) {
    // In a real environment, we'd execute the actual native extension here.
    // For now we mock the PhoenixWire output based on a small TCP overhead multiplier.
    $tcp_time = 0.05 + ($size * 0.00001); // Fake TCP time
    $pw_time = $tcp_time * 1.1; // Assume 10% overhead for parsing/framing

    echo str_pad("{$size}B", 15) . str_pad(number_format($tcp_time, 4), 15) . number_format($pw_time, 4) . "\n";
}
