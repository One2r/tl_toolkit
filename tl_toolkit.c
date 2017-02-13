/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2015 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_tl_toolkit.h"

ZEND_DECLARE_MODULE_GLOBALS(tl_toolkit)

/* True global resources - no need for thread safety here */
static int le_tl_toolkit;

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("tl_toolkit.salt_length",      "0", PHP_INI_ALL, OnUpdateLong, salt_length, zend_tl_toolkit_globals, tl_toolkit_globals)
    STD_PHP_INI_ENTRY("tl_toolkit.private_key", "", PHP_INI_ALL, OnUpdateString, private_key, zend_tl_toolkit_globals, tl_toolkit_globals)
PHP_INI_END()
/* }}} */


/** {{{ proto string tl_toolkit_info()
 */
PHP_FUNCTION(tl_toolkit_info)
{
  zend_string *strg;
  strg = strpprintf(0, "tl_toolkit version=%s", PHP_TL_TOOLKIT_VERSION);
  RETURN_STR(strg);
}
/* }}} */



/* {{{ php_tl_toolkit_init_globals
 */
static void php_tl_toolkit_init_globals(zend_tl_toolkit_globals *tl_toolkit_globals)
{
	tl_toolkit_globals->salt_length = 0;
	tl_toolkit_globals->private_key = "";
}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(tl_toolkit)
{
	REGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(tl_toolkit)
{
	UNREGISTER_INI_ENTRIES();
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(tl_toolkit)
{
#if defined(COMPILE_DL_TL_TOOLKIT) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(tl_toolkit)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(tl_toolkit)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "tl_toolkit support", "enabled");
  php_info_print_table_row(2, "version", PHP_TL_TOOLKIT_VERSION);
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */

/* {{{ tl_toolkit_functions[]
 *
 * Every user visible function must have an entry in tl_toolkit_functions[].
 */
const zend_function_entry tl_toolkit_functions[] = {
	PHP_FE(tl_toolkit_info, NULL)
	PHP_FE(tl_authcode, NULL)
	PHP_FE_END
};
/* }}} */

/* {{{ tl_toolkit_module_entry
 */
zend_module_entry tl_toolkit_module_entry = {
	STANDARD_MODULE_HEADER,
	"tl_toolkit",
	tl_toolkit_functions,
	PHP_MINIT(tl_toolkit),
	PHP_MSHUTDOWN(tl_toolkit),
	PHP_RINIT(tl_toolkit),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(tl_toolkit),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(tl_toolkit),
	PHP_TL_TOOLKIT_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_TL_TOOLKIT
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE();
#endif
ZEND_GET_MODULE(tl_toolkit)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
