<?php

namespace GCurl;

use GCurlHandle;

require_once __DIR__ . '/GCurlResponse.php';

class GCurlClient
{
    private ?GCurlHandle $handle = null;
    private string $browser;
    private array $defaultOptions = [];

    public function __construct(string $browser = 'chrome', array $defaultOptions = [])
    {
        $this->browser = $browser;
        $this->defaultOptions = $defaultOptions;
        $this->initHandle();
    }

    private function initHandle(): void
    {
        if ($this->handle) {
            gcurl_close($this->handle);
        }
        $this->handle = gcurl_init();
        gcurl_impersonate($this->handle, $this->browser);

        $merged = array_replace([
            GCURLOPT_RETURNTRANSFER => true,
            GCURLOPT_FOLLOWLOCATION => true,
            GCURLOPT_CONNECTTIMEOUT => 10,
            GCURLOPT_TIMEOUT        => 30,
        ], $this->defaultOptions);

        gcurl_setopt_array($this->handle, $merged);
    }

    public function setBrowser(string $browser): self
    {
        $this->browser = $browser;
        gcurl_impersonate($this->handle, $browser);
        return $this;
    }

    public function setProxy(string $proxyUrl, ?int $proxyType = null): self
    {
        gcurl_setopt($this->handle, GCURLOPT_PROXY, $proxyUrl);
        if ($proxyType !== null) {
            gcurl_setopt($this->handle, GCURLOPT_PROXYTYPE, $proxyType);
        }
        return $this;
    }

    public function setCookieJar(string $path): self
    {
        gcurl_setopt($this->handle, GCURLOPT_COOKIEFILE, $path);
        gcurl_setopt($this->handle, GCURLOPT_COOKIEJAR, $path);
        return $this;
    }

    public function setHeader(string $name, string $value): self
    {
        $headers = [$name . ': ' . $value];
        gcurl_setopt($this->handle, GCURLOPT_HTTPHEADER, $headers);
        return $this;
    }

    public function setHeaders(array $headers): self
    {
        gcurl_setopt($this->handle, GCURLOPT_HTTPHEADER, $headers);
        return $this;
    }

    public function get(string $url, array $options = []): GCurlResponse
    {
        gcurl_setopt($this->handle, GCURLOPT_URL, $url);
        gcurl_setopt($this->handle, GCURLOPT_HTTPGET, true);

        if (!empty($options)) {
            gcurl_setopt_array($this->handle, $options);
        }

        $body = gcurl_exec($this->handle);
        $info = gcurl_getinfo($this->handle);
        $err  = gcurl_error($this->handle);

        return new GCurlResponse($body !== false ? $body : '', $info, $err);
    }

    public function post(string $url, mixed $data = null, array $options = []): GCurlResponse
    {
        gcurl_setopt($this->handle, GCURLOPT_URL, $url);
        gcurl_setopt($this->handle, GCURLOPT_POST, true);

        if ($data !== null) {
            if (is_array($data)) {
                $data = http_build_query($data);
            }
            gcurl_setopt($this->handle, GCURLOPT_POSTFIELDS, $data);
        }

        if (!empty($options)) {
            gcurl_setopt_array($this->handle, $options);
        }

        $body = gcurl_exec($this->handle);
        $info = gcurl_getinfo($this->handle);
        $err  = gcurl_error($this->handle);

        return new GCurlResponse($body !== false ? $body : '', $info, $err);
    }

    public function getHandle(): GCurlHandle
    {
        return $this->handle;
    }

    public function __destruct()
    {
        if ($this->handle) {
            gcurl_close($this->handle);
            $this->handle = null;
        }
    }
}
