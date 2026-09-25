#!/bin/sh
# scripts/verify-fingerprint.sh
# Verifies TLS & HTTP/2 fingerprints for Chrome and Firefox targets

set -eu

echo "=========================================================="
echo " verifying TLS & HTTP/2 Fingerprints via tls.peet.ws "
echo "=========================================================="

CURL_BIN=""
if command -v curl-impersonate >/dev/null 2>&1; then
    CURL_BIN="curl-impersonate"
elif [ -x "/usr/local/bin/curl-impersonate" ]; then
    CURL_BIN="/usr/local/bin/curl-impersonate"
elif [ -x "/opt/gcurl/bin/curl-impersonate" ]; then
    CURL_BIN="/opt/gcurl/bin/curl-impersonate"
fi

if [ -n "${CURL_BIN}" ]; then
    echo "Using binary: ${CURL_BIN}"
    echo ""
    echo "--- 1. Testing Chrome impersonation (target: chrome124) ---"
    CHROME_OUT="$(${CURL_BIN} --impersonate chrome124 -s https://tls.peet.ws/api/all)"
    echo "${CHROME_OUT}" | grep -E '"(http_version|ja3|ja4)"' || echo "${CHROME_OUT}" | head -n 25

    echo ""
    echo "--- 2. Testing Firefox impersonation (target: firefox133) ---"
    FF_OUT="$(${CURL_BIN} --impersonate firefox133 -s https://tls.peet.ws/api/all)"
    echo "${FF_OUT}" | grep -E '"(http_version|ja3|ja4)"' || echo "${FF_OUT}" | head -n 25
else
    echo "curl-impersonate CLI not found. Testing with PHP if gcurl is available..."
fi

if php -m 2>/dev/null | grep -q gcurl; then
    echo ""
    echo "--- 3. Testing PHP gcurl extension ---"
    php -r "
        echo 'gcurl version: ' . phpversion('gcurl') . PHP_EOL;
    "
fi

echo ""
echo "=== Fingerprint verification script finished ==="
