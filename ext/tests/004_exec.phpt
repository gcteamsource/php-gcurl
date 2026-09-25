--TEST--
gcurl_exec() with RETURNTRANSFER
--SKIPIF--
<?php
if (!extension_loaded('gcurl')) die('skip gcurl not loaded');
?>
--FILE--
<?php

$ch = gcurl_init('https://tls.peet.ws/api/all');
gcurl_impersonate($ch, 'chrome');
gcurl_setopt($ch, GCURLOPT_RETURNTRANSFER, true);
gcurl_setopt($ch, GCURLOPT_TIMEOUT, 15);

$body = gcurl_exec($ch);
$info = gcurl_getinfo($ch);

var_dump(is_string($body));
var_dump($info['http_code'] === 200);
var_dump(gcurl_errno($ch) === 0);
var_dump(gcurl_error($ch) === '');

gcurl_close($ch);
echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
OK
