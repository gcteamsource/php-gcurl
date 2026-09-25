# gcurl — Custom TLS & HTTP/2 Impersonation Extension for PHP 8.4

> Native PHP 8.4 C extension wrapping `libcurl-impersonate` (BoringSSL & NSS) untuk mengatasi antibot modern (Cloudflare, Akamai, DataDome) dengan meniru ClientHello TLS (JA3/JA4) dan HTTP/2 fingerprint dari browser asli (Chrome, Firefox, Safari).

---

## 🚀 Fitur Utama

- **PHP 8.4 Native Extension (`gcurl`)**: API kompatibel dan identik dengan `curl_*` (`gcurl_init`, `gcurl_setopt`, `gcurl_exec`, `gcurl_multi_*`, dll).
- **Dual Engine Impersonation**:
  - **Chrome**: BoringSSL engine (mendukung TLS GREASE, ECH, ZSTD, X25519Kyber768, HTTP/2 SETTINGS identik).
  - **Firefox**: NSS engine (cipher ordering dan extension khas Firefox).
- **Concurrency & High Workload**: Full support untuk `gcurl_multi_*` asynchronous non-blocking event-loop (sangat cepat untuk scraping ribuan URL).
- **Proxy Support**: Mendukung SOCKS5, SOCKS5h (DNS resolve di proxy), HTTP, HTTPS proxy yang dapat di-manage langsung di kode PHP.
- **Bypass Cloudflare**: Integrasi mulus dengan Turnstile resolver eksternal (cukup inject `cf_clearance` dan User-Agent yang sama).
- **High-Level PHP Wrappers**: Disertakan class OOP `GCurlClient` dan `GCurlPool` di folder `src/`.

---

## 🛠️ Quick Start

### 1. Build Docker Image (Production Ready)
```bash
docker build -t gcurl:latest .
```

### 2. Jalankan Container
```bash
docker run --rm -it -v $(pwd):/var/www/html gcurl:latest sh
```

### 3. Jalankan Test Suite
```bash
# Menjalankan official PHP phpt test runner
docker run --rm -v $(pwd):/var/www/html gcurl:latest php /var/www/html/ext/run-tests.php -q /var/www/html/ext/tests/
```

---

## 📖 Contoh Penggunaan

### 1. Basic Request dengan Impersonasi Chrome
```php
$ch = gcurl_init('https://tls.peet.ws/api/all');

// Impersonasi Chrome (otomatis set TLS ciphers, curves, grease, dan HTTP/2 settings)
gcurl_impersonate($ch, 'chrome');

gcurl_setopt_array($ch, [
    GCURLOPT_RETURNTRANSFER => true,
    GCURLOPT_FOLLOWLOCATION => true,
    GCURLOPT_TIMEOUT        => 30,
]);

$response = gcurl_exec($ch);
$info = gcurl_getinfo($ch);
gcurl_close($ch);

echo "HTTP {$info['http_code']}\n";
$data = json_decode($response, true);
echo "JA3: " . $data['tls']['ja3'] . "\n";
```

### 2. Fallback ke Firefox & Residential Proxy
```php
$ch = gcurl_init('https://kasirpintar.com/login');

// Jika target mendeteksi Chrome, ganti ke Firefox
gcurl_impersonate($ch, 'firefox');

gcurl_setopt_array($ch, [
    GCURLOPT_RETURNTRANSFER => true,
    // Residential SOCKS5h proxy (DNS resolve di proxy)
    GCURLOPT_PROXY          => 'socks5h://user:password@proxy.example.com:1080',
    GCURLOPT_PROXYTYPE      => GCURLPROXY_SOCKS5,
    GCURLOPT_TIMEOUT        => 30,
]);

$response = gcurl_exec($ch);
gcurl_close($ch);
```

### 3. High-Concurrency Scraping dengan `GCurlPool`
```php
use GCurl\GCurlPool;
use GCurl\GCurlResponse;

require_once __DIR__ . '/src/GCurlPool.php';

$proxies = [
    'socks5h://user1:pass@proxy1:1080',
    'socks5h://user2:pass@proxy2:1080',
];

$pool = new GCurlPool(concurrency: 50, browser: 'chrome', proxies: $proxies);

for ($i = 0; $i < 500; $i++) {
    $pool->add("https://example.com/item/$i", function(GCurlResponse $res, string $url) {
        if ($res->isSuccess()) {
            echo "Fetched $url in {$res->totalTime()}s\n";
        }
    });
}

$pool->run();
```

---

## 📂 Struktur Project

```text
.
├── Dockerfile                      # Multi-stage production image (PHP 8.4 Alpine + gcurl)
├── docker-compose.yml              # Dev & testing service
├── ext/                            # PHP C Extension Source
│   ├── config.m4                   # Autotools build config
│   ├── php_gcurl.h                 # Headers, structs, macros
│   ├── gcurl.c                     # Module entry & class registration
│   ├── gcurl_handle.c              # Core handle, impersonate, setopt, exec
│   ├── gcurl_multi.c               # Concurrency multi-handle
│   ├── gcurl_share.c               # Share handle implementation
│   ├── gcurl_constants.c           # All GCURLOPT_* and GCURLINFO_* constants
│   └── tests/                      # .phpt unit test suite
├── src/                            # PHP OOP Wrappers
│   ├── GCurlClient.php             # Fluent OOP HTTP Client
│   ├── GCurlResponse.php           # Response DTO
│   └── GCurlPool.php               # High-volume worker pool with proxy rotation
├── examples/                       # Executable sample scripts
│   ├── test_single_handle.php      # Verifikasi single handle & impersonate
│   ├── test_multi_concurrency.php  # Verifikasi concurrency multi-handle
│   ├── test_oop_wrapper.php        # Verifikasi class GCurlClient & GCurlPool
│   └── login_kasirpintar.php       # Integrasi bypass Cloudflare kasirpintar.com
└── plan/                           # Dokumentasi & roadmap lengkap
```
