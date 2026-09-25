<?php

require_once __DIR__ . '/../src/GCurlClient.php';
require_once __DIR__ . '/../src/GCurlPool.php';

use GCurl\GCurlClient;
use GCurl\GCurlPool;
use GCurl\GCurlResponse;

echo "=== Testing GCurlClient ===" . PHP_EOL;

$client = new GCurlClient(browser: 'chrome');
$res = $client->get('https://tls.peet.ws/api/all');

echo "Client GET status: " . $res->statusCode() . PHP_EOL;
echo "Client total time: " . $res->totalTime() . "s" . PHP_EOL;
$json = $res->json();
echo "Client JA3: " . substr($json['tls']['ja3'] ?? '', 0, 30) . "..." . PHP_EOL;

echo PHP_EOL . "=== Testing GCurlPool (Concurrency) ===" . PHP_EOL;

$pool = new GCurlPool(concurrency: 4, browser: 'chrome');
$urls = [
    'https://tls.peet.ws/api/all',
    'https://tls.peet.ws/api/all',
    'https://tls.peet.ws/api/all',
    'https://tls.peet.ws/api/all',
];

$successCount = 0;
foreach ($urls as $i => $u) {
    $pool->add($u, function (GCurlResponse $resp, string $url) use (&$successCount, $i) {
        echo "Pool worker #$i completed -> HTTP " . $resp->statusCode() . PHP_EOL;
        if ($resp->isSuccess()) {
            $successCount++;
        }
    });
}

$pool->run();

echo PHP_EOL . "Pool completed: $successCount / " . count($urls) . " succeeded!" . PHP_EOL;
if ($successCount === count($urls)) {
    echo "✓ All OOP wrappers verified successfully!" . PHP_EOL;
}
