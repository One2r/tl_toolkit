/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_tl_toolkit.h"
#include "zend_API.h"
#include "ext/standard/md5.h"

#define PHP_TL_DE_AUTHCODE_DEFAULT_OP   "DECODE"
#define PHP_TL_DE_AUTHCODE_DEFAULT_KEY  "6713wj3NZqPxPILb7MyzF2nFGc3DNoSW9yWMRA"

/* {{{ tl_md5
 */
zend_string *tl_md5(zend_string *str,zend_bool raw_output)
{
    zend_string *result;
    char md5str[33];
    PHP_MD5_CTX context;
    unsigned char digest[16];

    md5str[0] = '\0';
    PHP_MD5Init(&context);
    PHP_MD5Update(&context, ZSTR_VAL(str), ZSTR_LEN(str));
    PHP_MD5Final(digest, &context);
    if (raw_output) {
        return zend_string_init((char *) digest, 16,0);
    } else {
        make_digest_ex(md5str, digest, 16);
       return zend_string_init(md5str,33,0);
    }
}
/* }}} */

/*{{ tl_de_authcode
 */
PHP_FUNCTION(tl_authcode)
{
    zend_string *input;
    zend_string *operate = zend_string_init(PHP_TL_DE_AUTHCODE_DEFAULT_OP, sizeof(PHP_TL_DE_AUTHCODE_DEFAULT_OP) - 1, 0);
    zend_string *key = zend_string_init(PHP_TL_DE_AUTHCODE_DEFAULT_KEY, sizeof(PHP_TL_DE_AUTHCODE_DEFAULT_KEY) - 1, 0);
    zend_long expiry = 0;
    zend_string *output = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_STR(input)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(operate)
        Z_PARAM_STR(key)
        Z_PARAM_LONG(expiry)
    ZEND_PARSE_PARAMETERS_END();
    output = tl_md5(input,0);
    RETURN_STR(output);
}
/* }}} */
