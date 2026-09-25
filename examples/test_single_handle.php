<?php

echo "=== Test 1: Single Handle & Chrome Impersonation ===" . PHP_EOL;

$ch = gcurl_init('https://tls.peet.ws/api/all');
var_dump($ch instanceof GCurlHandle);
var_dump(gcurl_impersonate($ch, 'chrome'));
var_dump(gcurl_setopt($ch, GCURLOPT_RETURNTRANSFER, true));
var_dump(gcurl_setopt($ch, GCURLOPT_TIMEOUT, 15));

$res = gcurl_exec($ch);
$info = gcurl_getinfo($ch);

echo "HTTP Code: " . $info['http_code'] . PHP_EOL;
echo "Total Time: " . $info['total_time'] . "s" . PHP_EOL;

$data = json_decode($res, true);
echo "HTTP Version: " . ($data['http_version'] ?? 'N/A') . PHP_EOL;
echo "Chrome JA3: " . ($data['tls']['ja3'] ?? 'N/A') . PHP_EOL;
echo "Chrome JA4: " . ($data['tls']['ja4'] ?? 'N/A') . PHP_EOL;
gcurl_close($ch);

echo PHP_EOL . "=== Test 2: Firefox Impersonation ===" . PHP_EOL;

$ch2 = gcurl_init('https://tls.peet.ws/api/all');
gcurl_impersonate($ch2, 'firefox');
gcurl_setopt_array($ch2, [
    GCURLOPT_RETURNTRANSFER => true,
    GCURLOPT_TIMEOUT => 15,
]);

$res2 = gcurl_exec($ch2);
$data2 = json_decode($res2, true);
echo "Firefox JA3: " . ($data2['tls']['ja3'] ?? 'N/A') . PHP_EOL;
echo "Firefox JA4: " . ($data2['tls']['ja4'] ?? 'N/A') . PHP_EOL;
gcurl_close($ch2);

echo PHP_EOL . "=== Test 3: Error Handling & Reset ===" . PHP_EOL;

$ch3 = gcurl_init('https://invalid-domain-does-not-exist-12345.xyz');
gcurl_setopt($ch3, GCURLOPT_RETURNTRANSFER, true);
gcurl_setopt($ch3, GCURLOPT_TIMEOUT, 5);
$res3 = gcurl_exec($ch3);
echo "Result on error: " . var_export($res3, true) . PHP_EOL;
echo "Errno: " . gcurl_errno($ch3) . PHP_EOL;
echo "Error string: " . gcurl_error($ch3) . PHP_EOL;
echo "Strerror: " . gcurl_strerror(gcurl_errno($ch3)) . PHP_EOL;

gcurl_reset($ch3);
echo "After reset errno: " . gcurl_errno($ch3) . PHP_EOL;
gcurl_close($ch3);

echo PHP_EOL . "=== Test 4: Copy Handle ===" . PHP_EOL;

$ch4 = gcurl_init('https://tls.peet.ws/api/all');
gcurl_impersonate($ch4, 'chrome');
gcurl_setopt($ch4, GCURLOPT_RETURNTRANSFER, true);

$copy = gcurl_copy_handle($ch4);
var_dump($copy instanceof GCurlHandle);
$copy_res = gcurl_exec($copy);
$copy_info = gcurl_getinfo($copy);
echo "Copy Handle HTTP Code: " . $copy_info['http_code'] . PHP_EOL;

gcurl_close($ch4);
gcurl_close($copy);

echo PHP_EOL . "=== ALL TESTS COMPLETED SUCCESSFULLY ===" . PHP_EOL;
