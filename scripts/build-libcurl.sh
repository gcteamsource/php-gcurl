#!/bin/sh
# scripts/build-libcurl.sh
# Downloads and prepares libcurl-impersonate musl build for Alpine Linux

set -eu

CURL_IMPERSONATE_VERSION="${CURL_IMPERSONATE_VERSION:-v2.0.0}"
INSTALL_DIR="${1:-/opt/gcurl}"

echo "=== Preparing libcurl-impersonate (${CURL_IMPERSONATE_VERSION}) for Alpine Linux (musl) ==="

ARCH="$(uname -m)"
case "${ARCH}" in
    x86_64)  MUSL_ARCH="x86_64" ;;
    aarch64) MUSL_ARCH="aarch64" ;;
    *)
        echo "Error: Unsupported architecture: ${ARCH}"
        exit 1
        ;;
esac

mkdir -p "${INSTALL_DIR}/bin" "${INSTALL_DIR}/lib" "${INSTALL_DIR}/include"
TMP_DIR="$(mktemp -d)"
trap 'rm -rf "${TMP_DIR}"' EXIT

LIB_TAR="libcurl-impersonate-${CURL_IMPERSONATE_VERSION}.${MUSL_ARCH}-linux-musl.tar.gz"
BIN_TAR="curl-impersonate-${CURL_IMPERSONATE_VERSION}.${MUSL_ARCH}-linux-musl.tar.gz"
BASE_URL="https://github.com/lexiforest/curl-impersonate/releases/download/${CURL_IMPERSONATE_VERSION}"

echo "Downloading ${LIB_TAR}..."
curl -fL -s "${BASE_URL}/${LIB_TAR}" -o "${TMP_DIR}/${LIB_TAR}"

echo "Extracting library and headers to ${INSTALL_DIR}..."
tar -zx -C "${TMP_DIR}" -f "${TMP_DIR}/${LIB_TAR}"
cp -a "${TMP_DIR}/include/"* "${INSTALL_DIR}/include/"
cp -a "${TMP_DIR}"/libcurl-impersonate* "${INSTALL_DIR}/lib/"

echo "Downloading CLI tools ${BIN_TAR}..."
if curl -fL -s "${BASE_URL}/${BIN_TAR}" -o "${TMP_DIR}/${BIN_TAR}"; then
    tar -zx -C "${INSTALL_DIR}/bin" -f "${TMP_DIR}/${BIN_TAR}"
    chmod 755 "${INSTALL_DIR}/bin"/* || true
fi

chmod 755 "${INSTALL_DIR}/lib"/libcurl-impersonate.so* || true

echo "=== libcurl-impersonate setup completed at ${INSTALL_DIR} ==="
ls -la "${INSTALL_DIR}/lib"
ls -la "${INSTALL_DIR}/include/curl"
