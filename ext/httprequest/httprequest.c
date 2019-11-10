/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
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

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_httprequest.h"
#include <time.h>


/* If you declare any globals in php_httprequest.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(httprequest)
*/

/* True global resources - no need for thread safety here */
static int le_httprequest;

zend_class_entry *http_request_ce;



/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("httprequest.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_httprequest_globals, httprequest_globals)
    STD_PHP_INI_ENTRY("httprequest.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_httprequest_globals, httprequest_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_httprequest_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_httprequest_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "httprequest", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/


/* {{{ php_httprequest_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_httprequest_init_globals(zend_httprequest_globals *httprequest_globals)
{
	httprequest_globals->global_value = 0;
	httprequest_globals->global_string = NULL;
}
*/
/* }}} */

/**
 * 获得IP
 */
char * getIp()
{
	zval *server = &PG(http_globals)[TRACK_VARS_SERVER];
	if (server != NULL) {
		zval *forwared = zend_hash_str_find(Z_ARR_P(server), ZEND_STRL("HTTP_X_FORWARDED_FOR"));
		if (forwared != NULL) return Z_STRVAL_P(forwared);

		zval *clientIp = zend_hash_str_find(Z_ARR_P(server), ZEND_STRL("HTTP_CLIENT_IP"));
		if (clientIp != NULL) return Z_STRVAL_P(clientIp);

		zval *remoteAddr = zend_hash_str_find(Z_ARR_P(server), ZEND_STRL("REMOTE_ADDR"));
		if (remoteAddr != NULL) return Z_STRVAL_P(remoteAddr);
	}

	return "";
}

/**
 * 返回浏览器信息
 */
char * getAgent()
{
	zval *server = &PG(http_globals)[TRACK_VARS_SERVER];
	if (server != NULL) {
		zval *userAgent = zend_hash_str_find(Z_ARR_P(server), ZEND_STRL("HTTP_USER_AGENT"));
		if (userAgent != NULL) return Z_STRVAL_P(userAgent);
	}
	return "";
}
/**
 * 判断是否为https
 */
int isHttps()
{
	zval *server = &PG(http_globals)[TRACK_VARS_SERVER];
	zval *https = zend_hash_str_find(Z_ARR_P(server), ZEND_STRL("HTTPS"));
	if (https != NULL) {
		return (strcmp(Z_STRVAL_P(https) , "on") == 0) || (strcmp(Z_STRVAL_P(https) , "ON") == 0);
	}

	return 0;
}


/**
 * 初始化参数
 */
void initHttp(zval *thisval)
{
	zval *params = zend_read_property(http_request_ce,thisval,ZEND_STRL("_params"),0 ,NULL);

	if (Z_TYPE_P(params) == IS_NULL) {
		array_init(params);
	}

	zval *referer = NULL;
	zval *request_time = NULL;


	zval *server = &PG(http_globals)[TRACK_VARS_SERVER];
	if (server != NULL) {
		referer = zend_hash_str_find(HASH_OF(server), ZEND_STRL("HTTP_REFERER"));
		request_time = zend_hash_str_find(HASH_OF(server), ZEND_STRL("REQUEST_TIME"));
	}

	//获得时间戳
	time_t seconds;
	seconds = request_time != NULL ? Z_LVAL_P(request_time) : time(NULL);
	char *rf = referer != NULL ? Z_STRVAL_P(referer) : "";
	add_assoc_long(params,"REQUEST_TIME",seconds);

	//HTTP_REFERER 设置param
	add_assoc_stringl_ex(params,ZEND_STRL("HTTP_REFERER"),rf , strlen(rf));

	//获得ip,设置param
	char *ip = getIp();
	add_assoc_stringl_ex(params,ZEND_STRL("CLIENT_IP"),ip ,strlen(ip));

	//获得浏览器信息,设置param
	char *agent = getAgent();
	add_assoc_stringl_ex(params,ZEND_STRL("CLIENT_AGENT"),agent ,strlen(agent));

	//更新params参数
	zend_update_property(http_request_ce,thisval,ZEND_STRL("_params"),params);

	//更新is_https参数
	zend_update_property_bool(http_request_ce,thisval, ZEND_STRL("_is_https"), isHttps());
}
/**
 * 初始化
 */
PHP_METHOD(HttpRequest,__construct)
{
	initHttp(getThis());
}

/**
 * 实现单例模式
 */
PHP_METHOD(HttpRequest,current)
{
	if (PG(auto_globals_jit)) {
		zend_string *server_str = zend_string_init(ZEND_STRL("_SERVER"), 0);
		zend_is_auto_global(server_str);
		zend_string_release(server_str);
	}

	//获取static参数_current
	zval *this_ptr = zend_read_static_property(http_request_ce  ,ZEND_STRL("_current") ,1);
	if (Z_ISNULL_P(this_ptr)) {
		//实例化对象
		object_init_ex(this_ptr, http_request_ce);
		//设置static参数_current
		zend_update_static_property(http_request_ce ,ZEND_STRL("_current")  ,this_ptr);
	}
	initHttp(this_ptr);
	RETURN_ZVAL(this_ptr, 1, 0);
}

/**
 * 获取参数方法
 */
zval * getParam(zval *this_ptr , zend_string *key)
{
	//获得自己设置的参数
	zval *params = zend_read_property(http_request_ce,this_ptr,ZEND_STRL("_params"),0 ,NULL);
	zval *value  = zend_hash_find(Z_ARR_P(params), key);
	if (value != NULL) {
		return value;
	}

	//获得get内信息
	if (PG(auto_globals_jit)) {
		zend_string *getstr = zend_string_init(ZEND_STRL("_GET"), 0);
		zend_is_auto_global(getstr);
		zend_string_release(getstr);
	}
	zval *get  = &PG(http_globals)[TRACK_VARS_GET];
	value  = zend_hash_find(HASH_OF(get), key);
	if (value != NULL) {
		return value;
	}

	//获得post内信息
	if (PG(auto_globals_jit)) {
		zend_string *poststr = zend_string_init(ZEND_STRL("_POST"), 0);
		zend_is_auto_global(poststr);
		zend_string_release(poststr);
	}
	zval *post  = &PG(http_globals)[TRACK_VARS_POST];
	value  = zend_hash_find(Z_ARR_P(post), key);
	if (value != NULL) {
		return value;
	}

	//获得cookie内信息
	if (PG(auto_globals_jit)) {
		zend_string *cookiestr = zend_string_init(ZEND_STRL("_COOKIE"), 0);
		zend_is_auto_global(cookiestr);
		zend_string_release(cookiestr);
	}
	zval *cookie  = &PG(http_globals)[TRACK_VARS_COOKIE];
	value  = zend_hash_find(Z_ARR_P(cookie), key);
	if (value != NULL) {
		return value;
	}

	//获得request内信息
	if (PG(auto_globals_jit)) {
		zend_string *requeststr = zend_string_init(ZEND_STRL("_REQUEST"), 0);
		zend_is_auto_global(requeststr);
		zend_string_release(requeststr);
	}
	zval *request = &PG(http_globals)[TRACK_VARS_REQUEST];
	value  = zend_hash_find(Z_ARR_P(request), key);
	if (value != NULL) {
		return value;
	}

	if (PG(auto_globals_jit)) {
		zend_string *envstr = zend_string_init(ZEND_STRL("_ENV"), 0);
		zend_is_auto_global(envstr);
		zend_string_release(envstr);
	}
	zval *env  = &PG(http_globals)[TRACK_VARS_ENV];
	value  = zend_hash_find(Z_ARR_P(env), key);
	if (value != NULL) {
		return value;
	}
	return NULL;
}
/**
 * 设置参数
 */
void setParam(zval *thisval ,zval *key ,zval *value)
{
	zval *params = zend_read_property(http_request_ce,thisval,ZEND_STRL("_params"),0 ,NULL);

	if (Z_TYPE_P(params) == IS_NULL) {
		array_init(params);
	}

	add_assoc_zval(params,Z_STRVAL_P(key),value);
	zend_update_property(http_request_ce,thisval,ZEND_STRL("_params"),params);
}
/**
 * 获取参数(魔法函数)
 */
PHP_METHOD(HttpRequest,__get)
{
	zend_string *key;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		if (key == NULL) {
			php_error_docref(NULL, E_WARNING, "Parameter cannot be empty!");
		}
		return;
	}

	zval  *value = getParam(getThis(),key);
	if (value != NULL) {
		RETURN_ZVAL(value, 0, 1);
	} else {
		RETURN_NULL();
	}

}

/**
 * 获取参数
 */
PHP_METHOD(HttpRequest,get)
{
	zend_string *key;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &key) == FAILURE) {
		if (key == NULL) {
			php_error_docref(NULL, E_WARNING, "Parameter cannot be empty!");
		}
		return;
	}

	zval  *value = getParam(getThis(),key);
	if (value != NULL) {
		RETURN_ZVAL(value, 0, 1);
	} else {
		RETURN_NULL();
	}
}

/**
 *设置参数（魔法函数）
 */
PHP_METHOD(HttpRequest,__set)
{
	zval *key   = NULL;
	zval *value = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &key,&value) == FAILURE) {
		if (Z_STRVAL_P(key) == NULL || Z_STRVAL_P(value) == NULL) {
			php_error_docref(NULL, E_WARNING, "Parameter cannot be empty!");
		}
		return;
	}

	if (Z_TYPE_P(key) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "key type is error,not is string!");
		return ;
	}
	setParam(getThis(),key,value);
}

/**
 *设置参数
 */
PHP_METHOD(HttpRequest,set)
{
	zval *key   = NULL;
	zval *value = NULL;
	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|zz", &key,&value) == FAILURE) {
		if (Z_STRVAL_P(key) == NULL || Z_STRVAL_P(value) == NULL) {
			php_error_docref(NULL, E_WARNING, "Parameter cannot be empty!");
		}
		return;
	}
	if (Z_TYPE_P(key) != IS_STRING) {
		php_error_docref(NULL, E_WARNING, "key type is error,not is string!");
		return ;
	}
	setParam(getThis(),key,value);
	zval *this_ptr = zend_read_static_property(http_request_ce  ,ZEND_STRL("_current") ,1);
	RETURN_ZVAL(this_ptr, 1, 0);
}


/**
 * 是否是https方法
 */
PHP_METHOD(HttpRequest,isHttps)
{
	RETURN_BOOL(isHttps());
}

/**
 * 是否是post方法
 */
PHP_METHOD(HttpRequest,isPost)
{
	zval *server = &PG(http_globals)[TRACK_VARS_SERVER];

	zval *method = zend_hash_str_find(HASH_OF(server), ZEND_STRL("REQUEST_METHOD"));
	if (method == NULL) {
		RETURN_BOOL(false);
	} else {
		RETURN_BOOL(strcmp(Z_STRVAL_P(method) ,"POST") == 0);
	}

}

/**
 * 获得浏览器信息
 */
PHP_METHOD(HttpRequest,getAgent)
{
	char *value = getAgent();
	RETURN_STRING(value);
}

/**
 * 获得IP
 */
PHP_METHOD(HttpRequest,getIp)
{
	char *value = getIp();
	RETURN_STRING(value);
}


ZEND_BEGIN_ARG_INFO_EX(global_set_arg, 0, 0, 2)
    ZEND_ARG_INFO(0, key)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(global_get_arg, 0, 0, 1)
    ZEND_ARG_INFO(0, key)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(global_getSecurity_arg, 0, 0, 1)
    ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

zend_function_entry http_request_methods[] = {
	PHP_ME(HttpRequest,__construct, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(HttpRequest,__get, global_get_arg ,ZEND_ACC_PUBLIC | ZEND_ACC_USE_GUARDS)
	PHP_ME(HttpRequest,get, global_get_arg, ZEND_ACC_PUBLIC )
	PHP_ME(HttpRequest,__set, global_set_arg, ZEND_ACC_PUBLIC | ZEND_ACC_USE_GUARDS)
	PHP_ME(HttpRequest,set, global_set_arg, ZEND_ACC_PUBLIC )
	PHP_ME(HttpRequest, current, NULL, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC )
	PHP_ME(HttpRequest, isHttps, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(HttpRequest, isPost, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(HttpRequest, getAgent, NULL, ZEND_ACC_PUBLIC)
	PHP_ME(HttpRequest, getIp, NULL, ZEND_ACC_PUBLIC)
	{NULL, NULL, NULL}
};

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(httprequest)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/


	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "HttpRequest", http_request_methods);
	http_request_ce = zend_register_internal_class_ex(&ce, NULL);

	zend_declare_property_null(http_request_ce, ZEND_STRL("_params"),  ZEND_ACC_PROTECTED);
	zend_declare_property_bool(http_request_ce, ZEND_STRL("_is_https"), 0, ZEND_ACC_PROTECTED);
	zend_declare_property_null(http_request_ce, ZEND_STRL("_current"), ZEND_ACC_PRIVATE | ZEND_ACC_STATIC);

	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(httprequest)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(httprequest)
{
#if defined(COMPILE_DL_HTTPREQUEST) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(httprequest)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(httprequest)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "httprequest support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ httprequest_functions[]
 *
 * Every user visible function must have an entry in httprequest_functions[].
 */
const zend_function_entry httprequest_functions[] = {
	PHP_FE(confirm_httprequest_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in httprequest_functions[] */
};
/* }}} */

/* {{{ httprequest_module_entry
 */
zend_module_entry httprequest_module_entry = {
	STANDARD_MODULE_HEADER,
	"httprequest",
	httprequest_functions,
	PHP_MINIT(httprequest),
	PHP_MSHUTDOWN(httprequest),
	PHP_RINIT(httprequest),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(httprequest),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(httprequest),
	PHP_HTTPREQUEST_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_HTTPREQUEST
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(httprequest)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
