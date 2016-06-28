/** {{{ proto string tl_toolkit_info_v()
 */
PHP_FUNCTION(tl_toolkit_info_v)
{
    zend_string *strg;
	strg = strpprintf(0, "tl_toolkit version=%s", PHP_TL_TOOLKIT_VERSION);

	RETURN_STR(strg);
}
/* }}} */


