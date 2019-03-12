--TEST--
Test function tl_get_arch of tl_toolkit

--SKIPIF--
<?php if (!extension_loaded("tl_toolkit")) print "skip"; ?>

--FILE--
<?php
echo tl_get_arch();
?>

--EXPECT--
64
