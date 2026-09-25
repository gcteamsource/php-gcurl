--TEST--
gcurl Chrome vs Firefox fingerprint verification
--SKIPIF--
<?php
if (!extension_loaded('gcurl')) die('skip gcurl not loaded');
?>
--FILE--
<?php

function fetch_fingerprint(string $browser, int $retries = 3): ?array {
    for ($i = 0; $i < $retries; $i++) {
        $ch = gcurl_init('https://tls.peet.ws/api/all');
        gcurl_impersonate($ch, $browser);
        gcurl_setopt($ch, GCURLOPT_RETURNTRANSFER, true);
        gcurl_setopt($ch, GCURLOPT_TIMEOUT, 15);
        $res = gcurl_exec($ch);
        gcurl_close($ch);
        if ($res) {
            $json = json_decode($res, true);
            if (isset($json['http_version'], $json['tls']['ja3'])) {
                return $json;
            }
        }
        usleep(500000);
    }
    return null;
}

$chrome_json = fetch_fingerprint('chrome');
$firefox_json = fetch_fingerprint('firefox');

var_dump(is_array($chrome_json));
var_dump(is_array($firefox_json));
var_dump($chrome_json['http_version'] === 'h2');
var_dump($firefox_json['http_version'] === 'h2');
var_dump(strpos($chrome_json['tls']['ja3'], '771,') === 0);
var_dump(strpos($firefox_json['tls']['ja3'], '771,') === 0);
var_dump($chrome_json['tls']['ja3'] !== $firefox_json['tls']['ja3']);

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
