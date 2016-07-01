/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_tl_toolkit.h"

/** {{{ proto string tl_str_highlight()
 */
PHP_FUNCTION(tl_str_highlight)
{
    zend_string *strg;
	strg = strpprintf(0, "tl_toolkit version=%s", PHP_TL_TOOLKIT_VERSION);

	RETURN_STR(strg);
}
/* }}} */


