/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_tl_toolkit.h"
#include "ext/standard/md5.h"

/*{{ tl_de_authcode
 */
PHP_FUNCTION(tl_de_authcode)
{
    zend_string *input;
    zend_string *operate = zend_string_init("DECODE", sizeof("DECODE") - 1, 0);
    zend_string *key = zend_string_init("6713wj3NZqPxPILb7MyzF2nFGc3DNoSW9yWMRA", sizeof("6713wj3NZqPxPILb7MyzF2nFGc3DNoSW9yWMRA") - 1, 0);
    int expiry = 0;
    zend_string *output = NULL;

    if (zend_parse_parameters(ZEND_NUM_ARGS(), "S|S|S|d",&input, &operate, &key, &expiry) == FAILURE) {
        RETURN_NULL();
    }
    //zend_string *md5key = _tl_md5(key);

    RETVAL_STR(input);
}
/* }}} */
