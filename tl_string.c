/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php_tl_toolkit.h"
#include "zend_API.h"
#include "ext/standard/md5.h"
#include "ext/standard/base64.h"

#define PHP_TL_AUTHCODE_DEFAULT_OP   "DECODE"

/* {{{ tl_md5
 */
zend_string *tl_md5(zend_string *str,zend_bool raw_output)
{
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


/*{{ tl_authcode
 */
PHP_FUNCTION(tl_authcode)
{
    zend_string *input;
    zend_string *operate = zend_string_init(PHP_TL_AUTHCODE_DEFAULT_OP, sizeof(PHP_TL_AUTHCODE_DEFAULT_OP) - 1, 0);
    zend_string *key = zend_string_init(INI_STR("tl_toolkit.private_key"), strlen(INI_STR("tl_toolkit.private_key")), 0);
    zend_long expiry = 0;
    int salt_length = INI_INT("tl_toolkit.salt_length");
    zend_string *output = NULL;
    ZEND_PARSE_PARAMETERS_START(1, 3)
        Z_PARAM_STR(input)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR(operate)
        Z_PARAM_LONG(expiry)
    ZEND_PARSE_PARAMETERS_END();

    key = tl_md5(key,0);
    zend_string *key_a = tl_md5(zend_string_init(ZSTR_VAL(key),16,0),0);
    zend_string *key_b = tl_md5(zend_string_init(ZSTR_VAL(key) + 16,16,0),0);
    efree(key);
    zend_string *key_c;
    if(strcmp(ZSTR_VAL(operate), PHP_TL_AUTHCODE_DEFAULT_OP) == 0){
        key_c = zend_string_init(ZSTR_VAL(input),salt_length,0);
    }else{
        zval funcname;
        zval microtime;
        ZVAL_STRING(&funcname,"microtime");
        call_user_function(CG(function_table), NULL, &funcname, &microtime, 0, NULL);
        zend_string *md5microtime = tl_md5(zend_string_init(Z_STRVAL(microtime),Z_STRLEN(microtime),0),0);
        key_c = zend_string_init(
                ZSTR_VAL(md5microtime)+((ZSTR_LEN(md5microtime)-salt_length))-1,
                salt_length,
                0
                );
        efree(md5microtime);
    }

    zend_string *zstr_keyac = zend_string_init(ZSTR_VAL(key_a),ZSTR_LEN(key_a)+ZSTR_LEN(key_c),0);
    strcat(ZSTR_VAL(zstr_keyac),ZSTR_VAL(key_c));
    zend_string *zstr_md5keyac = tl_md5(zstr_keyac,0);
    efree(zstr_keyac);
    zend_string *cryptkey = zend_string_init(ZSTR_VAL(key_a),ZSTR_LEN(key_a)+ZSTR_LEN(zstr_md5keyac),0);
    strcat(ZSTR_VAL(cryptkey),ZSTR_VAL(zstr_md5keyac));
    efree(zstr_md5keyac);

    if(strcmp(ZSTR_VAL(operate), PHP_TL_AUTHCODE_DEFAULT_OP) == 0){
        int length =  ZSTR_LEN(input) - salt_length;
        unsigned char *base64_code = ZSTR_VAL(input)+salt_length;
        input = php_base64_decode(base64_code,length);
    }else{
        if(expiry != 0){
            expiry += (zend_long)time(NULL);
        }
        int i_len = ZSTR_LEN(input) + 26;
        char c_time_str[i_len];
        php_sprintf(c_time_str,"%010d",expiry);

        zend_string *zstr_input_new = zend_string_init(ZSTR_VAL(input),ZSTR_LEN(input)+ZSTR_LEN(key_b),0);
        strcat(ZSTR_VAL(zstr_input_new),ZSTR_VAL(key_b));
        zend_string *zstr_input_keyb = tl_md5(zstr_input_new,0);
        efree(zstr_input_new);

        zend_string *zstr_sub_input_keyb = zend_string_init(ZSTR_VAL(zstr_input_keyb),16,0);
        efree(zstr_input_keyb);
        strcat(c_time_str,ZSTR_VAL(zstr_sub_input_keyb));
        efree(zstr_sub_input_keyb);
        strcat(c_time_str,ZSTR_VAL(input));

        input = zend_string_init(c_time_str , strlen(c_time_str),0);
    }

    int rndkey[256];
    int box[256];

    int i;
    for(i=0;i<256;i++){
        box[i] = i;
        rndkey[i] = (int)ZSTR_VAL(cryptkey)[i % ZSTR_LEN(cryptkey)];
    }

    int j,tmp;
    for(j=i=0;i<256;i++){
        j = (j + i + rndkey[i]) % 256;
        tmp = box[i];
        box[i] = box[j];
        box[j] = tmp;
    }

    int k,ord_int;
    unsigned char ord_str_p[ZSTR_LEN(input)];
    for(k=j=i=0;i<ZSTR_LEN(input);i++){
        k = (k + 1) % 256;
        j = (j + box[k]) % 256;

        tmp = box[k];
        box[k] = box[j];
        box[j] = tmp;

        ord_int = (int)ZSTR_VAL(input)[i];
        ord_str_p[i] = (unsigned char)(ord_int ^ (box[(box[k] + box[j]) % 256]));
    }
    output = zend_string_init(ord_str_p,ZSTR_LEN(input),0);
    efree(input);
    if(strcmp(ZSTR_VAL(operate), PHP_TL_AUTHCODE_DEFAULT_OP) == 0){
        zend_string *sub_output_a = zend_string_init(ZSTR_VAL(output),10,0);
        zend_long expiry_code = ZEND_STRTOL(ZSTR_VAL(sub_output_a),NULL,10);
        efree(sub_output_a);
        zend_string *sub_output_b = zend_string_init(ZSTR_VAL(output)+10,16,0);
        zend_string *sub_output_c = zend_string_init(ZSTR_VAL(output)+26,ZSTR_LEN(output)+ZSTR_LEN(key_b)-26,0);
        strcat(ZSTR_VAL(sub_output_c),ZSTR_VAL(key_b));
        efree(sub_output_b);
        zend_string *md5_sub_output_c = tl_md5(sub_output_c,0);
        efree(sub_output_c);
        md5_sub_output_c = zend_string_init(ZSTR_VAL(md5_sub_output_c),16,0);
        efree(key_a);
        efree(key_b);
        efree(key_c);
        if( (expiry_code==0 || expiry_code - (zend_long)time(NULL) >0) && strcmp(ZSTR_VAL(sub_output_b),ZSTR_VAL(md5_sub_output_c)) == 0  ){
            efree(md5_sub_output_c);
            RETURN_STR(zend_string_init(ZSTR_VAL(output)+26,ZSTR_LEN(output)-26,0));
        }else{
            RETURN_NULL();
        }
    }else{
        output = php_base64_encode((unsigned char *)ZSTR_VAL(output),ZSTR_LEN(output));
        char *pch;
        do{
          pch= strstr(ZSTR_VAL(output),"=");
          if(pch == NULL) break;
          strncpy (pch,"",1);
        }while(1);
        efree(pch);
        zend_string *tmp_z_output = zend_string_init(ZSTR_VAL(key_c),ZSTR_LEN(key_c)+ZSTR_LEN(output),0);
        strcat(ZSTR_VAL(tmp_z_output),ZSTR_VAL(output));
        output = tmp_z_output;
        efree(key_a);
        efree(key_b);
        efree(key_c);
        RETURN_STR(output);
    }
}
/* }}} */
