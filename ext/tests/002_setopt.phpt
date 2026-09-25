--TEST--
gcurl_setopt() and gcurl_setopt_array() basic tests
--SKIPIF--
<?php
if (!extension_loaded('gcurl')) die('skip gcurl not loaded');
?>
--FILE--
<?php

$ch = gcurl_init();

var_dump(gcurl_setopt($ch, GCURLOPT_RETURNTRANSFER, true));
var_dump(gcurl_setopt($ch, GCURLOPT_URL, 'https://example.com'));

var_dump(gcurl_setopt_array($ch, [
    GCURLOPT_FOLLOWLOCATION => true,
    GCURLOPT_TIMEOUT        => 30,
    GCURLOPT_MAXREDIRS      => 5,
]));

var_dump(gcurl_setopt($ch, GCURLOPT_PROXY, 'socks5h://proxy.example.com:1080'));
var_dump(gcurl_setopt($ch, GCURLOPT_PROXYTYPE, GCURLPROXY_SOCKS5));

var_dump(GCURLOPT_URL === CURLOPT_URL);
var_dump(GCURLOPT_RETURNTRANSFER === 19913);

gcurl_close($ch);
echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
OK
