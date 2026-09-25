<?php

/** @generate-class-entries */

/**
 * @not-serializable
 */
final class GCurlHandle {}

/**
 * @not-serializable
 */
final class GCurlMultiHandle {}

/**
 * @not-serializable
 */
final class GCurlShareHandle {}

function gcurl_version(?int $age = null): array|false {}

function gcurl_init(?string $url = null): GCurlHandle|false {}

function gcurl_impersonate(GCurlHandle $handle, string $target, bool $default_headers = true): bool {}

function gcurl_setopt(GCurlHandle $handle, int $option, mixed $value): bool {}

function gcurl_setopt_array(GCurlHandle $handle, array $options): bool {}

function gcurl_exec(GCurlHandle $handle): string|bool {}

function gcurl_getinfo(GCurlHandle $handle, ?int $option = null): mixed {}

function gcurl_errno(GCurlHandle $handle): int {}

function gcurl_error(GCurlHandle $handle): string {}

function gcurl_strerror(int $error_code): ?string {}

function gcurl_reset(GCurlHandle $handle): void {}

function gcurl_close(GCurlHandle $handle): void {}

function gcurl_copy_handle(GCurlHandle $handle): GCurlHandle|false {}

function gcurl_multi_init(): GCurlMultiHandle {}

function gcurl_multi_add_handle(GCurlMultiHandle $multi_handle, GCurlHandle $handle): int {}

function gcurl_multi_remove_handle(GCurlMultiHandle $multi_handle, GCurlHandle $handle): int {}

function gcurl_multi_exec(GCurlMultiHandle $multi_handle, int &$still_running): int {}

function gcurl_multi_select(GCurlMultiHandle $multi_handle, float $timeout = 1.0): int {}

function gcurl_multi_info_read(GCurlMultiHandle $multi_handle, ?int &$queued_messages = null): array|false {}

function gcurl_multi_getcontent(GCurlHandle $handle): ?string {}

function gcurl_multi_errno(GCurlMultiHandle $multi_handle): int {}

function gcurl_multi_strerror(int $error_code): ?string {}

function gcurl_multi_close(GCurlMultiHandle $multi_handle): void {}

function gcurl_multi_setopt(GCurlMultiHandle $multi_handle, int $option, mixed $value): bool {}

function gcurl_share_init(): GCurlShareHandle {}

function gcurl_share_setopt(GCurlShareHandle $share_handle, int $option, mixed $value): bool {}

function gcurl_share_close(GCurlShareHandle $share_handle): void {}
