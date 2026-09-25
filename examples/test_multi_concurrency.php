<?php
/**
 * examples/test_multi_concurrency.php
 * Demonstrates high-workload concurrent scraping with gcurl_multi_*
 */

$targets = [
    ['url' => 'https://tls.peet.ws/api/all', 'browser' => 'chrome'],
    ['url' => 'https://tls.peet.ws/api/all', 'browser' => 'firefox'],
    ['url' => 'https://tls.peet.ws/api/all', 'browser' => 'chrome124'],
    ['url' => 'https://tls.peet.ws/api/all', 'browser' => 'firefox133'],
    ['url' => 'https://tls.peet.ws/api/all', 'browser' => 'chrome'],
    ['url' => 'https://tls.peet.ws/api/all', 'browser' => 'firefox'],
];

echo "==========================================================" . PHP_EOL;
echo " Testing Concurrent Scraping with gcurl_multi_* (" . count($targets) . " requests) " . PHP_EOL;
echo "==========================================================" . PHP_EOL;

$startTime = microtime(true);

$mh = gcurl_multi_init();
$handles = [];

foreach ($targets as $i => $item) {
    $ch = gcurl_init($item['url']);
    gcurl_impersonate($ch, $item['browser']);
    gcurl_setopt_array($ch, [
        GCURLOPT_RETURNTRANSFER => true,
        GCURLOPT_FOLLOWLOCATION => true,
        GCURLOPT_TIMEOUT        => 20,
        GCURLOPT_CONNECTTIMEOUT => 10,
    ]);

    gcurl_multi_add_handle($mh, $ch);
    $handles[$i] = [
        'ch' => $ch,
        'browser' => $item['browser'],
    ];
}

$still_running = null;
do {
    $status = gcurl_multi_exec($mh, $still_running);
    if ($still_running) {
        gcurl_multi_select($mh, 0.05);
    }
} while ($still_running > 0 && $status === GCURLM_OK);

$elapsed = microtime(true) - $startTime;

echo "Concurrent execution finished in " . sprintf("%.2f", $elapsed) . "s" . PHP_EOL . PHP_EOL;

$completedCount = 0;
foreach ($handles as $i => $item) {
    $ch = $item['ch'];
    $body = gcurl_multi_getcontent($ch);
    $info = gcurl_getinfo($ch);

    $json = json_decode($body, true);
    $ja3 = substr($json['tls']['ja3'] ?? 'N/A', 0, 40) . '...';
    $httpVer = $json['http_version'] ?? 'N/A';

    echo sprintf(
        "Request #%d [%-10s] -> HTTP %d (%s) | JA3: %s\n",
        $i + 1,
        $item['browser'],
        $info['http_code'],
        $httpVer,
        $ja3
    );

    if ($info['http_code'] === 200) {
        $completedCount++;
    }

    gcurl_multi_remove_handle($mh, $ch);
    gcurl_close($ch);
}

gcurl_multi_close($mh);

echo PHP_EOL . "Summary: $completedCount / " . count($targets) . " requests succeeded!" . PHP_EOL;
if ($completedCount === count($targets)) {
    echo "✓ High-concurrency gcurl_multi is working perfectly!" . PHP_EOL;
}
