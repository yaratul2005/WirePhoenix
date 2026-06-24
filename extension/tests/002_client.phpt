--TEST--
Check PhoenixWire\Client class functionality
--SKIPIF--
<?php
if (!extension_loaded('phoenixwire')) {
    echo 'skip';
}
?>
--FILE--
<?php
$client = new PhoenixWire\Client();
var_dump($client->getState()); // PW_STATE_DISCONNECTED = 0
$client->connect("127.0.0.1", 8080);
var_dump($client->getState()); // PW_STATE_CONNECTING = 1
?>
--EXPECT--
int(0)
int(1)
