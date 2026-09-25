--TEST--
gcurl_init() and gcurl_close() basic test
--SKIPIF--
<?php
if (!extension_loaded('gcurl')) die('skip gcurl not loaded');
?>
--FILE--
<?php

$ch = gcurl_init();
var_dump($ch instanceof GCurlHandle);

$ch2 = gcurl_init('https://example.com');
var_dump($ch2 instanceof GCurlHandle);

gcurl_close($ch);
gcurl_close($ch2);

echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
OK
