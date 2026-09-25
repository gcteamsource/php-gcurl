# gcurl — PHP Extension for Browser Fingerprint Impersonation

[![CI](https://github.com/greatcode/gcurl/actions/workflows/ci.yml/badge.svg)](https://github.com/greatcode/gcurl/actions/workflows/ci.yml)
[![PIE Compatible](https://img.shields.io/badge/PIE-Compatible-blue.svg?style=flat-square)](https://github.com/php/pie)
[![Latest Version on Packagist](https://img.shields.io/packagist/v/greatcode/gcurl.svg?style=flat-square)](https://packagist.org/packages/greatcode/gcurl)
[![PHP Version](https://img.shields.io/badge/php-%3E%3D%208.2-8892BF.svg?style=flat-square)](https://php.net)
[![License: MIT](https://img.shields.io/badge/License-MIT-blue.svg?style=flat-square)](LICENSE)

**gcurl** is a high-performance native PHP C extension wrapping [`libcurl-impersonate`](https://github.com/lexiforest/curl-impersonate). It mimics the exact **TLS (JA3 / JA4)** and **HTTP/2** network fingerprints of modern web browsers (**Chrome** and **Firefox**), enabling scraping and API requests to bypass advanced antibot and WAF protections such as Cloudflare Turnstile, Akamai, and DataDome.

Distributed modernly via **[PIE (PHP Installer for Extensions)](https://github.com/php/pie)** and **Composer**.

---

## Features

- **True TLS Handshake Spoofing (JA3/JA4)**: Replaces OpenSSL's standard ClientHello with BoringSSL (Chrome) or NSS (Firefox) cipher ordering, extensions, curves (X25519Kyber768), and ECH.
- **HTTP/2 Fingerprint Matching**: Accurate HTTP/2 `SETTINGS`, `WINDOW_UPDATE`, and frame priorities.
- **`curl_*` Compatible API**: Drop-in procedural functions (`gcurl_init`, `gcurl_setopt`, `gcurl_exec`, etc.) alongside `gcurl_impersonate($ch, 'chrome'|'firefox')`.
- **High-Level PHP SDK**: Fluent OOP HTTP client (`Greatcode\Gcurl\GCurlClient`) and async concurrency pool (`Greatcode\Gcurl\GCurlPool`).
- **PHP 8.2, 8.3, 8.4 Support**: Native Zend Object class architecture (`GCurlHandle`, `GCurlMultiHandle`, `GCurlShareHandle`).
- **Residential Proxy Ready**: Seamless pass-through for HTTP, SOCKS5, and SOCKS5h proxies via standard `GCURLOPT_PROXY`.

---

## Installation

### 1. Install C Extension via PIE (Recommended)

[PIE](https://github.com/php/pie) is the official, modern PHP extension installer created by The PHP Foundation (replacing PECL):

```bash
# Install PIE if not already installed (https://github.com/php/pie)
pie install greatcode/gcurl
```

> **Note**: Ensure `libcurl-impersonate` is installed on your system (`/usr/local` or standard library path). If installed in a custom location:
> ```bash
> pie install greatcode/gcurl --with-gcurl=/path/to/libcurl-impersonate
> ```

---

### 2. Install PHP SDK (Composer)

Install the PHP client and concurrency pool via [Packagist](https://packagist.org/packages/greatcode/gcurl):

```bash
composer require greatcode/gcurl
```

---

### 3. Alternative: Prebuilt Binaries

Pre-compiled binary releases are available from [GitHub Releases](https://github.com/greatcode/gcurl/releases):

| Platform | Architectures | libc | Download |
| :--- | :--- | :--- | :--- |
| **Linux (Ubuntu / Debian / RHEL)** | `x86_64`, `aarch64` | glibc | [Latest Release](https://github.com/greatcode/gcurl/releases) |
| **Alpine Linux (Docker)** | `x86_64`, `aarch64` | musl | [Latest Release](https://github.com/greatcode/gcurl/releases) |

Extract and run the included installer:
```bash
tar -xzf gcurl-v0.1.0-php8.4-linux-glibc-x86_64.tar.gz
cd gcurl-v0.1.0-php8.4-linux-glibc-x86_64
sudo ./install.sh
```

---

### 4. Alternative: Build from Source

```bash
# Clone the repository
git clone https://github.com/greatcode/gcurl.git
cd gcurl/ext

# Prepare build environment
phpize
./configure --with-gcurl=/usr/local
make -j$(nproc)
sudo make install

# Enable the extension in php.ini
echo "extension=gcurl.so" | sudo tee -a $(php -r 'echo php_ini_loaded_file();')
```

> **Note for Alpine/Linux with system libcurl**: When using `libcurl-impersonate`, set `ENV LD_PRELOAD=/usr/local/lib/libcurl-impersonate.so` in your environment or Dockerfile to ensure PHP resolves symbols to the impersonation engine.

---

## Quickstart

### 1. Fluent OOP Client (`Greatcode\Gcurl\GCurlClient`)

```php
<?php

require_once __DIR__ . '/vendor/autoload.php';

use Greatcode\Gcurl\GCurlClient;

$client = new GCurlClient(browser: 'chrome');

// Optional: Set residential proxy
// $client->setProxy('socks5h://user:pass@proxy.example.com:1080');

$response = $client->get('https://tls.peet.ws/api/all');

echo "HTTP Code: " . $response->statusCode() . PHP_EOL;
$json = $response->json();
echo "JA3 Fingerprint: " . $json['tls']['ja3'] . PHP_EOL;
```

---

### 2. High-Concurrency Pool (`Greatcode\Gcurl\GCurlPool`)

For large-scale scraping with automatic proxy rotation:

```php
<?php

require_once __DIR__ . '/vendor/autoload.php';

use Greatcode\Gcurl\GCurlPool;
use Greatcode\Gcurl\GCurlResponse;

$proxies = [
    'socks5h://proxy1.example.com:1080',
    'socks5h://proxy2.example.com:1080',
];

$pool = new GCurlPool(concurrency: 20, browser: 'chrome', proxies: $proxies);

for ($i = 1; $i <= 100; $i++) {
    $pool->add("https://httpbin.org/get?id={$i}", function (GCurlResponse $res, string $url) {
        echo "Finished {$url} -> Status " . $res->statusCode() . PHP_EOL;
    });
}

// Execute parallel requests non-blocking
$pool->run();
```

---

### 3. Procedural API (`gcurl_*`)

If you prefer standard `curl_*` procedural syntax:

```php
<?php

$ch = gcurl_init();

// 1. Set browser impersonation BEFORE other options
gcurl_impersonate($ch, 'chrome'); // or 'firefox'

gcurl_setopt_array($ch, [
    GCURLOPT_URL            => 'https://tls.peet.ws/api/all',
    GCURLOPT_RETURNTRANSFER => true,
    GCURLOPT_ENCODING       => '', // Auto-decompress gzip/br/zstd
    GCURLOPT_TIMEOUT        => 15,
]);

$response = gcurl_exec($ch);

if (gcurl_errno($ch)) {
    echo "Error: " . gcurl_error($ch) . PHP_EOL;
} else {
    echo $response;
}

gcurl_close($ch);
```

---

## Supported Browser Profiles

Pass any of the following targets to `gcurl_impersonate($ch, $target)` or `new GCurlClient(browser: $target)`:

| Profile | Engine | Target Names |
| :--- | :--- | :--- |
| **Chrome** | BoringSSL | `'chrome'` *(alias to latest)*, `'chrome131'`, `'chrome124'`, `'chrome120'`, `'chrome116'` |
| **Firefox** | NSS | `'firefox'` *(alias to latest)*, `'firefox133'`, `'firefox117'` |
| **Safari** | BoringSSL | `'safari'`, `'safari180'` |
| **Edge** | BoringSSL | `'edge'`, `'edge101'` |

---

## Cloudflare Turnstile & Challenge Bypass Workflow

When scraping protected endpoints:

```
┌──────────────────────────────────────────────┐
│ Turnstile Solver (External)                  │
│ Solves widget -> yields cf_clearance + UA    │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
┌──────────────────────────────────────────────┐
│ gcurl Script                                 │
│ 1. gcurl_impersonate($ch, 'chrome')          │
│ 2. Set Cookie: cf_clearance=...              │
│ 3. Set User-Agent matching solver            │
│ 4. Send Request via residential proxy        │
└──────────────────────┬───────────────────────┘
                       │
                       ▼
               200 OK (Bypassed! 🎉)
```

```php
$client = new GCurlClient(browser: 'chrome');
$client->setHeader('User-Agent', $turnstileUserAgent);
$client->setHeader('Cookie', 'cf_clearance=' . $cfClearance);
$client->setProxy($residentialProxy);

$res = $client->get('https://example.com/protected-page');
```

---

## Contributing

Pull requests are welcome! For major changes, please open an issue first to discuss what you would like to change.

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Run tests (`cd ext && make test`)
4. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
5. Push to the Branch (`git push origin feature/AmazingFeature`)
6. Open a Pull Request

---

## License

This project is licensed under the [MIT License](LICENSE) - see the LICENSE file for details.
