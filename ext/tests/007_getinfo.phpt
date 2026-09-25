--TEST--
gcurl_getinfo() returns all keys
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
gcurl_exec($ch);

$info = gcurl_getinfo($ch);

$keys = [
    'url', 'http_code', 'total_time', 'namelookup_time',
    'connect_time', 'pretransfer_time', 'starttransfer_time',
    'redirect_count', 'size_download', 'header_size',
    'request_size', 'content_type', 'primary_ip', 'scheme',
    'http_version', 'errno', 'error'
];

$all_present = true;
foreach ($keys as $k) {
    if (!array_key_exists($k, $info)) {
        echo "Missing key: $k\n";
        $all_present = false;
    }
}
var_dump($all_present);
var_dump($info['http_code'] === 200);
var_dump($info['scheme'] === 'https');
var_dump(gcurl_getinfo($ch, GCURLINFO_HTTP_CODE) === 200);

gcurl_close($ch);
echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
OK
