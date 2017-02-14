--TEST--
Test function tl_authcode of tl_toolkit
--SKIPIF--
<?php if (!extension_loaded("tl_toolkit")) print "skip"; ?>
--FILE--
<?php
$str = "tl_authcode";
$code = tl_authcode($str,"ENCODE");
echo tl_authcode($code);
/*
	you can add regression tests for your extension here

  the output of your test code has to be equal to the
  text in the --EXPECT-- section below for the tests
  to pass, differences between the output and the
  expected text are interpreted as failure

	see php7/README.TESTING for further information on
  writing regression tests
*/
?>
--EXPECT--
tl_authcode
