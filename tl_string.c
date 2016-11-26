/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_tl_toolkit.h"
#include "zend_API.h"
#include "ext/standard/md5.h"
#include "ext/standard/base64.h"

#define PHP_TL_AUTHCODE_DEFAULT_OP   "DECODE"
#define PHP_TL_AUTHCODE_DEFAULT_KEY  "6713wj3NZqPxPILb7MyzF2nFGc3DNoSW9yWMRA"
#define PHP_TL_AUTHCODE_CKEY_LENGTH  4

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

/** {{{ tl_concat_function
 */
zend_string *tl_concat_function()
{

}
/* }}} */

/*{{ tl_authcode
 */
PHP_FUNCTION(tl_authcode)
{
    zend_string *input;
    zend_string *operate = zend_string_init(PHP_TL_AUTHCODE_DEFAULT_OP, sizeof(PHP_TL_AUTHCODE_DEFAULT_OP) - 1, 0);
    zend_string *key = zend_string_init(PHP_TL_AUTHCODE_DEFAULT_KEY, sizeof(PHP_TL_AUTHCODE_DEFAULT_KEY) - 1, 0);
    zend_long expiry = 0;
    zend_string *output = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 4)
        Z_PARAM_STR(input)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(operate)
        Z_PARAM_STR(key)
        Z_PARAM_LONG(expiry)
    ZEND_PARSE_PARAMETERS_END();

    key = tl_md5(key,0);
    zend_string *key_a = tl_md5(zend_string_init(ZSTR_VAL(key),16,0),0);
    zend_string *key_b = tl_md5(zend_string_init(ZSTR_VAL(key) + 16,16,0),0);
    zend_string *key_c;
    zval funcname;
    if(strcmp(ZSTR_VAL(operate), PHP_TL_AUTHCODE_DEFAULT_OP) == 0){
        key_c = zend_string_init(ZSTR_VAL(input),PHP_TL_AUTHCODE_CKEY_LENGTH,0);
    }else{
        zval microtime;
        ZVAL_STRING(&funcname,"microtime");
        call_user_function(CG(function_table), NULL, &funcname, &microtime, 0, NULL);
        zend_string *md5microtime = tl_md5(zend_string_init(Z_STRVAL(microtime),Z_STRLEN(microtime),0),0);
        key_c = zend_string_init(
                ZSTR_VAL(md5microtime)+(ZSTR_LEN(md5microtime)-PHP_TL_AUTHCODE_CKEY_LENGTH),
                PHP_TL_AUTHCODE_CKEY_LENGTH,
                0
                );

    }

    zval z_keyac,z_keya_md5keyac;
    zval z_key_a,z_key_c,z_md5keyac;
    ZVAL_STR_COPY(&z_key_a, key_a);
    ZVAL_STR_COPY(&z_key_c, key_c);
    concat_function(&z_keyac, &z_key_a, &z_key_c);
    zend_string *md5keyac = tl_md5(zend_string_init(Z_STRVAL(z_keyac),Z_STRLEN(z_keyac),0),0);
    ZVAL_STR_COPY(&z_md5keyac,md5keyac);
    concat_function(&z_keya_md5keyac,&z_key_a,&z_md5keyac);
    zend_string *cryptkey = Z_STR(z_keya_md5keyac);

    if(strcmp(ZSTR_VAL(operate), PHP_TL_AUTHCODE_DEFAULT_OP) == 0){
        input = php_base64_decode((unsigned char*)zend_string_init(ZSTR_VAL(input)+PHP_TL_AUTHCODE_CKEY_LENGTH,ZSTR_LEN(input),0),ZSTR_LEN(input)-PHP_TL_AUTHCODE_CKEY_LENGTH);
    }else{
        if(expiry != 0){
            expiry += (zend_long)time(NULL);
        }
        char time_str[10];
        php_sprintf(time_str,"%010d",expiry);
        zval z_input,z_key_b,z_input_keyb,z_sub_input_keyb,z_time,z_input_tmp;
        ZVAL_STR_COPY(&z_input, input);
        ZVAL_STR_COPY(&z_key_b, key_b);
        concat_function(&z_input_keyb, &z_input, &z_key_b);
        zend_string *input_keyb = tl_md5(zend_string_init(Z_STRVAL(z_input_keyb),Z_STRLEN(z_input_keyb),0),0);
        zend_string *sub_input_keyb = zend_string_init(ZSTR_VAL(input_keyb),16,0);
        ZVAL_STR_COPY(&z_sub_input_keyb, sub_input_keyb);
        ZVAL_STRING(&z_time,time_str);
        concat_function(&z_input_tmp,&z_time,&z_sub_input_keyb);
        concat_function(&z_input,&z_input_tmp,&z_input);
        input = Z_STR(z_input);
    }

    int rndkey[256];

    int a,i,j,tmp,box[256];
    zval ord_str,z_output,z_output_tmp;
    zval argv[1];
    for(i=0;i<256;i++){
        box[i] = i;
        ZVAL_STRING(&funcname,"ord");
        ZVAL_STRING(&argv[0], &ZSTR_VAL(cryptkey)[i % ZSTR_LEN(cryptkey)]);
        call_user_function(CG(function_table), NULL, &funcname, &ord_str, 1, argv);
        rndkey[i] = Z_DVAL(ord_str);
    }

    for(i=0,j=0;i<256;i++){
        j = (j + i + rndkey[i]) % 256;
        tmp = box[i];
        box[i] = box[j];
        box[j] = tmp;
    }

    for(a=0,i=0,j=0;i<ZSTR_LEN(input);i++){
        a = (a + 1) % 256;
        j = (j + box[a]) % 256;
        tmp = box[a];
        box[a] = box[j];
        box[j] = tmp;
        ZVAL_STRING(&funcname,"ord");
        ZVAL_STRING(&argv[0], &ZSTR_VAL(input)[i]);
        call_user_function(CG(function_table), NULL, &funcname, &ord_str, 1, argv);

        ZVAL_STRING(&funcname,"chr");
        ZVAL_LONG(&argv[0], (int)Z_DVAL(ord_str) ^ (box[(box[a] + box[j]) % 256]));
        call_user_function(CG(function_table), NULL, &funcname, &ord_str, 1, argv);
        ZVAL_STRING(&z_output_tmp, Z_STRVAL(z_output));
        concat_function(&z_output,&z_output_tmp,&ord_str);
    }

    output = Z_STR(z_output);
    RETURN_STR(output);
}
/* }}} */
