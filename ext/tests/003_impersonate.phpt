--TEST--
gcurl_impersonate() Chrome and Firefox
--SKIPIF--
<?php
if (!extension_loaded('gcurl')) die('skip gcurl not loaded');
?>
--FILE--
<?php

$ch = gcurl_init();

// Test Chrome target alias
var_dump(gcurl_impersonate($ch, 'chrome'));

// Test Firefox target alias
gcurl_reset($ch);
var_dump(gcurl_impersonate($ch, 'firefox'));

// Test specific Chrome version
gcurl_reset($ch);
var_dump(gcurl_impersonate($ch, 'chrome124'));

// Test specific Firefox version
gcurl_reset($ch);
var_dump(gcurl_impersonate($ch, 'firefox133'));

// Test without default headers
gcurl_reset($ch);
var_dump(gcurl_impersonate($ch, 'chrome', false));

// Invalid target returns false
$result = @gcurl_impersonate($ch, 'nonexistent_browser_target_xyz_123');
var_dump($result === false);

gcurl_close($ch);
echo "OK\n";
?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
OK
