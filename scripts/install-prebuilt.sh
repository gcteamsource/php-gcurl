#!/usr/bin/env bash
set -e

echo "=== Installing gcurl extension ==="

if [ "$EUID" -ne 0 ]; then
  echo "Error: Please run as root or with sudo."
  exit 1
fi

if ! command -v php &> /dev/null; then
  echo "Error: 'php' command not found."
  exit 1
fi

EXT_DIR=$(php -r 'echo ini_get("extension_dir");')
INI_DIR=$(php --ini | grep "Scan this dir for additional .ini files" | cut -d: -f2 | xargs)

if [ -z "$EXT_DIR" ]; then
  echo "Error: Could not determine PHP extension directory."
  exit 1
fi

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

echo "Installing gcurl.so into $EXT_DIR..."
cp "${SCRIPT_DIR}/gcurl.so" "$EXT_DIR/"

if [ -f "${SCRIPT_DIR}/libcurl-impersonate.so" ]; then
  echo "Installing libcurl-impersonate.so into /usr/local/lib..."
  cp "${SCRIPT_DIR}/libcurl-impersonate.so"* /usr/local/lib/
  if command -v ldconfig &> /dev/null; then
    ldconfig /usr/local/lib 2>/dev/null || true
  fi
fi

if [ -n "$INI_DIR" ] && [ -d "$INI_DIR" ]; then
  echo "Writing configuration to $INI_DIR/20-gcurl.ini..."
  echo "extension=gcurl.so" > "$INI_DIR/20-gcurl.ini"
else
  LOADED_INI=$(php --ini | grep "Loaded Configuration File" | cut -d: -f2 | xargs)
  echo "Notice: Additional ini directory not found."
  echo "Please add 'extension=gcurl.so' to: $LOADED_INI"
fi

echo "Verifying installation..."
if php -m | grep -qi gcurl; then
  echo "✓ gcurl extension successfully enabled in CLI!"
else
  echo "Warning: Extension not yet active in CLI. Check php.ini settings."
fi
