--TEST--
Test function tl_authcode of tl_toolkit
--SKIPIF--
<?php if (!extension_loaded("tl_toolkit")) print "skip"; ?>
--INI--
[tl_toolkit]
tl_toolkit.salt_length = 4
tl_toolkit.expiry = 0
tl_toolkit.private_key = 6713wj3NZqPxPILb7MyzF2nFGc3DNoSW9yWMRA

--FILE--
<?php
$str = "tl_authcode";
$code = tl_authcode($str,"ENCODE");
var_dump( $str == tl_authcode($code) );
?>
--EXPECT--
bool(true)
