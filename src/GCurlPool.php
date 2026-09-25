<?php

namespace Greatcode\Gcurl;

use GCurlMultiHandle;

require_once __DIR__ . '/GCurlResponse.php';

class GCurlPool
{
    private GCurlMultiHandle $multi;
    private array $queue = [];
    private array $handles = [];
    private int $concurrency;
    private string $browser;
    private array $proxies = [];
    private int $proxyIndex = 0;

    public function __construct(int $concurrency = 25, string $browser = 'chrome', array $proxies = [])
    {
        $this->concurrency = $concurrency;
        $this->browser     = $browser;
        $this->proxies     = $proxies;
        $this->multi       = gcurl_multi_init();
    }

    public function add(string $url, callable $callback, array $options = []): self
    {
        $this->queue[] = [
            'url'      => $url,
            'callback' => $callback,
            'options'  => $options,
        ];
        return $this;
    }

    public function setProxies(array $proxies): self
    {
        $this->proxies = array_values($proxies);
        $this->proxyIndex = 0;
        return $this;
    }

    private function getNextProxy(): ?string
    {
        if (empty($this->proxies)) {
            return null;
        }
        $proxy = $this->proxies[$this->proxyIndex % count($this->proxies)];
        $this->proxyIndex++;
        return $proxy;
    }

    public function run(): void
    {
        $this->fillPool();

        $active = null;
        do {
            $status = gcurl_multi_exec($this->multi, $active);

            while ($info = gcurl_multi_info_read($this->multi)) {
                if ($info['msg'] === GCURLMSG_DONE) {
                    $ch  = $info['handle'];
                    $key = spl_object_id($ch);

                    if (isset($this->handles[$key])) {
                        $entry    = $this->handles[$key];
                        $body     = gcurl_multi_getcontent($ch);
                        $response = new GCurlResponse(
                            $body ?? '',
                            gcurl_getinfo($ch),
                            gcurl_error($ch)
                        );

                        ($entry['callback'])($response, $entry['url']);

                        gcurl_multi_remove_handle($this->multi, $ch);
                        gcurl_close($ch);
                        unset($this->handles[$key]);

                        $this->fillPool();
                    }
                }
            }

            if ($active) {
                gcurl_multi_select($this->multi, 0.05);
            }
        } while ($active > 0 || !empty($this->queue));
    }

    private function fillPool(): void
    {
        while (count($this->handles) < $this->concurrency && !empty($this->queue)) {
            $entry = array_shift($this->queue);

            $ch = gcurl_init($entry['url']);
            gcurl_impersonate($ch, $this->browser);

            $opts = array_replace([
                GCURLOPT_RETURNTRANSFER => true,
                GCURLOPT_FOLLOWLOCATION => true,
                GCURLOPT_CONNECTTIMEOUT => 10,
                GCURLOPT_TIMEOUT        => 30,
            ], $entry['options']);

            if (!isset($opts[GCURLOPT_PROXY])) {
                $proxy = $this->getNextProxy();
                if ($proxy !== null) {
                    $opts[GCURLOPT_PROXY] = $proxy;
                }
            }

            gcurl_setopt_array($ch, $opts);
            gcurl_multi_add_handle($this->multi, $ch);

            $this->handles[spl_object_id($ch)] = $entry + ['handle' => $ch];
        }
    }

    public function __destruct()
    {
        foreach ($this->handles as $entry) {
            gcurl_multi_remove_handle($this->multi, $entry['handle']);
            gcurl_close($entry['handle']);
        }
        gcurl_multi_close($this->multi);
    }
}
