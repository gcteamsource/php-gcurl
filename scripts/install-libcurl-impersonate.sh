#!/usr/bin/env bash
# scripts/install-libcurl-impersonate.sh
# Downloads and installs libcurl-impersonate headers and libraries into standard directory structure

set -euo pipefail

VERSION="${1:-v2.0.0}"
PREFIX="${2:-/usr/local}"

ARCH="$(uname -m)"
case "${ARCH}" in
  x86_64|amd64) ARCH="x86_64" ;;
  aarch64|arm64) ARCH="aarch64" ;;
  *) echo "Error: Unsupported architecture: ${ARCH}"; exit 1 ;;
esac

# Detect libc (musl on Alpine vs glibc on Ubuntu/Debian)
LIBC="linux-gnu"
if [ -f /etc/alpine-release ] || (ldd /bin/ls 2>&1 | grep -qi "musl"); then
  LIBC="linux-musl"
fi

PKG_NAME="libcurl-impersonate-${VERSION}.${ARCH}-${LIBC}.tar.gz"
URL="https://github.com/lexiforest/curl-impersonate/releases/download/${VERSION}/${PKG_NAME}"

echo "=== Installing libcurl-impersonate (${PKG_NAME}) into ${PREFIX} ==="

TMP_DIR="$(mktemp -d)"
trap 'rm -rf "${TMP_DIR}"' EXIT

curl -fsSL "${URL}" -o "${TMP_DIR}/${PKG_NAME}"
tar -xz -C "${TMP_DIR}" -f "${TMP_DIR}/${PKG_NAME}"

# Ensure standard directory layout: headers in include/ and shared libs in lib/
mkdir -p "${PREFIX}/include" "${PREFIX}/lib"
cp -r "${TMP_DIR}/include/"* "${PREFIX}/include/"
cp -a "${TMP_DIR}"/libcurl-impersonate* "${PREFIX}/lib/"
chmod 755 "${PREFIX}"/lib/libcurl-impersonate.so* 2>/dev/null || true

if command -v ldconfig &> /dev/null; then
  ldconfig "${PREFIX}/lib" 2>/dev/null || true
fi

echo "✓ libcurl-impersonate successfully installed:"
ls -lh "${PREFIX}/lib"/libcurl-impersonate*
ls -lh "${PREFIX}/include/curl/curl.h"
