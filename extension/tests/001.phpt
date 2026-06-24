--TEST--
Check if phoenixwire is loaded
--SKIPIF--
<?php
if (!extension_loaded('phoenixwire')) {
    echo 'skip';
}
?>
--FILE--
<?php
echo 'The extension "phoenixwire" is available';
?>
--EXPECT--
The extension "phoenixwire" is available
