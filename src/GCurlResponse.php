<?php

namespace GCurl;

class GCurlResponse
{
    public function __construct(
        private readonly string $body,
        private readonly array  $info,
        private readonly string $error,
    ) {}

    public function body(): string
    {
        return $this->body;
    }

    public function statusCode(): int
    {
        return (int)($this->info['http_code'] ?? 0);
    }

    public function isSuccess(): bool
    {
        $code = $this->statusCode();
        return $code >= 200 && $code < 300;
    }

    public function info(?string $key = null): mixed
    {
        if ($key !== null) {
            return $this->info[$key] ?? null;
        }
        return $this->info;
    }

    public function error(): string
    {
        return $this->error;
    }

    public function json(bool $associative = true): mixed
    {
        return json_decode($this->body, $associative);
    }

    public function totalTime(): float
    {
        return (float)($this->info['total_time'] ?? 0.0);
    }

    public function primaryIp(): string
    {
        return (string)($this->info['primary_ip'] ?? '');
    }
}
