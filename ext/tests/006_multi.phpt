--TEST--
gcurl_multi_* concurrent requests
--SKIPIF--
<?php
if (!extension_loaded('gcurl')) die('skip gcurl not loaded');
?>
--FILE--
<?php

$mh = gcurl_multi_init();
var_dump($mh instanceof GCurlMultiHandle);

$ch1 = gcurl_init('https://tls.peet.ws/api/all');
gcurl_impersonate($ch1, 'chrome');
gcurl_setopt($ch1, GCURLOPT_RETURNTRANSFER, true);
gcurl_setopt($ch1, GCURLOPT_TIMEOUT, 15);

$ch2 = gcurl_init('https://tls.peet.ws/api/all');
gcurl_impersonate($ch2, 'firefox');
gcurl_setopt($ch2, GCURLOPT_RETURNTRANSFER, true);
gcurl_setopt($ch2, GCURLOPT_TIMEOUT, 15);

var_dump(gcurl_multi_add_handle($mh, $ch1) === GCURLM_OK);
var_dump(gcurl_multi_add_handle($mh, $ch2) === GCURLM_OK);

$still_running = null;
do {
    $status = gcurl_multi_exec($mh, $still_running);
    if ($still_running) {
        gcurl_multi_select($mh, 0.1);
    }
} while ($still_running > 0 && $status === GCURLM_OK);

$completed = 0;
while ($info = gcurl_multi_info_read($mh)) {
    if ($info['msg'] === GCURLMSG_DONE) {
        $completed++;
    }
}
var_dump($completed === 2);

$res1 = gcurl_multi_getcontent($ch1);
$res2 = gcurl_multi_getcontent($ch2);

$json1 = json_decode($res1, true);
$json2 = json_decode($res2, true);

var_dump($json1['http_version'] === 'h2');
var_dump($json2['http_version'] === 'h2');
var_dump($json1['tls']['ja3'] !== $json2['tls']['ja3']);

gcurl_multi_remove_handle($mh, $ch1);
gcurl_multi_remove_handle($mh, $ch2);
gcurl_close($ch1);
gcurl_close($ch2);
gcurl_multi_close($mh);

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
