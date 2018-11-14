/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
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
#include "php_plame.h"
#include "../include/lame.v2.h"

/* If you declare any globals in php_plame.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(plame)
*/

/* True global resources - no need for thread safety here */
static int le_plame;

#define EXT_ARRAY_INIT 							zval *arr = NULL; ALLOC_INIT_ZVAL(arr); array_init(arr)
#define EXT_ARRAY_ADD_STRING(Key, Val)			add_assoc_stringl(arr, Key, Val, strlen(Val), 1)
#define EXT_ARRAY_RETURN						RETURN_ZVAL(arr, 1, 1)

#define EXT_OBJECT_INIT							zval *obj = NULL; MAKE_STD_ZVAL(obj); object_init(obj)
#define EXT_OBJECT_ADD_STRING(Key, Val)			add_property_stringl(obj, Key, Val, strlen(Val), 1)
#define EXT_OBJECT_RETURN						RETURN_ZVAL(obj, 1, 1)

#define EXT_COPY_STRING(_zval, _val)			ZVAL_STRING(_zval, _val, 1)
#define EXT_COPY_ZVAL(_zval, _val)				ZVAL_COPY_VALUE(_zval, _val); zval_copy_ctor(_zval)
#define EXT_COPY_LONG(_zval, _val)				ZVAL_LONG(_zval, _val)
#define EXT_SET_PARAM(func, _zval, _val, _arg)	MAKE_STD_ZVAL(_zval); func(_zval, (_val)); _arg = &_zval

#define EXT_SET_STRING_PARAM(_zval, _val, _arg)	EXT_SET_PARAM(EXT_COPY_STRING, _zval, _val, _arg)
#define EXT_SET_LONG_PARAM(_zval, _val, _arg)	EXT_SET_PARAM(EXT_COPY_LONG, _zval, _val, _arg)
#define EXT_SET_ZVAL_PARAM(_zval, _val, _arg)	EXT_SET_PARAM(EXT_COPY_ZVAL, _zval, _val, _arg)
#define EXT_SET_ZOBJ_PARAM(_zval, _val, _arg)	EXT_SET_PARAM(construct_scan_result, _zval, _val, _arg)
#define EXT_DESTORY_PARAM(args, n)				do{int i; for(i=0; i<n; i++) {zval_ptr_dtor(args[i]);}} while(0)
#define EXT_RETURN_DETECT_RESULT(_val)			zval* zdr = NULL; MAKE_STD_ZVAL(zdr); construct_scan_result(zdr, _val); RETURN_ZVAL(zdr, 1, 1)

static inline zval* construct_scan_result(zval *zdr, lame_scan_result* result)
{
	object_init(zdr);
	add_property_long(zdr, "mklass", result->mklass);
	add_property_stringl(zdr, "kclass", result->kclass, strlen(result->kclass), 1);
	add_property_stringl(zdr, "kclass_desc_a", result->kclass_desc_a, strlen(result->kclass_desc_a), 1);
	// add_property_stringl(zdr, "kclass_desc_w", result->kclass_desc_w, sizoeof(result->kclass_desc_w), 1);
	add_property_stringl(zdr, "engid", result->engid, strlen(result->engid), 1);
	add_property_stringl(zdr, "vname", result->vname, strlen(result->vname), 1);
	add_property_long(zdr, "vid32", result->vid32);
	add_property_long(zdr, "vid40", result->vid40);
	add_property_stringl(zdr, "hitag", result->hitag, strlen(result->hitag), 1);
	add_property_long(zdr, "treat", result->treat);
}

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("plame.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_plame_globals, plame_globals)
    STD_PHP_INI_ENTRY("plame.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_plame_globals, plame_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_plame_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_plame_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "plame", arg);
	RETURN_STRINGL(strg, len, 0);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/

/* {{{ proto long
	lame_open_vdb(string vlibf)
 */
PHP_FUNCTION(lame_open_vdb)
{
	char *vlibf = NULL;
	int vlibf_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s!", &vlibf, &vlibf_len) == FAILURE) {
		RETRUN_NULL();
	}
	void* vdb = lame_open_vdb(vlibf);
	RETURN_LONG((long)vdb);
}
/* }}} */

/* {{{ proto void
	lame_close_vdb(long vdb)
 */
PHP_FUNCTION(lame_close_vdb)
{
	long vdb;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &vdb) == FAILURE) {
		return;
	}
	lame_close_vdb(vdb);
}
/* }}} */

/* {{{ proto long
	lame_create(long vdb)
 */
PHP_FUNCTION(lame_create)
{
	long vdb;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &vdb) == FAILURE) {
		return;
	}
	void* lame = lame_create(vdb);
	RETURN_LONG((long)lame);
}
/* }}} */

/* {{{ proto void
	lame_destroy(long lame)
 */
PHP_FUNCTION(lame_destroy)
{
	long lame;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &lame) == FAILURE) {
		return;
	}
	lame_destroy(lame);
}
/* }}} */

/* {{{ proto long
	lame_fork(long lame)
 */
PHP_FUNCTION(lame_fork)
{
	long lame;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &lame) == FAILURE) {
		return;
	}
	void* new_lame = lame_fork(lame);
	RETURN_LONG((long)new_lame);
}
/* }}} */

/* {{{ proto long
	lame_param_set(long lame, string param)
 */
PHP_FUNCTION(lame_param_set)
{
	char* parameter = NULL;
	long lame, parameter_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &lame, &parameter, &parameter_len) == FAILURE) {
		return;
	}
	long result = lame_param_set(lame, parameter);
	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto long
	lame_init(long lame)
 */
PHP_FUNCTION(lame_init)
{
	long lame;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &lame) == FAILURE) {
		return;
	}
	long result = lame_init(lame);
	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto object
	lame_scan_file(long lame, string fname)
 */
PHP_FUNCTION(lame_scan_file)
{
	char* filename = NULL;
	long lame, filename_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &lame, &filename, &filename_len) == FAILURE) {
		return;
	}
	lame_scan_result result;
	if(0 != lame_scan_file(lame, filename, &result)) {
		return;
	}
	EXT_RETURN_DETECT_RESULT(&result);
}
/* }}} */

/* {{{ proto object
	lame_scan_mem(long lame, string data)
 */
PHP_FUNCTION(lame_scan_mem)
{
	char* data = NULL;
	long lame, data_len;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ls", &lame, &data, &data_len) == FAILURE) {
		return;
	}
	lame_scan_result result;
	if(0 != lame_scan_mem(lame, data, data_len, &result)) {
		return;
	}
	EXT_RETURN_DETECT_RESULT(&result);
}
/* }}} */

typedef struct local_cb_data
{
	union fci {
		struct scan {
			zend_fcall_info ef;
			zend_fcall_info_cache ef_cache;
			zend_fcall_info lf;
			zend_fcall_info_cache lf_cache;
			zend_fcall_info al;
			zend_fcall_info_cache al_cache;
		} scan;
		struct extract {
			zend_fcall_info ef;
			zend_fcall_info_cache ef_cache;
			zend_fcall_info lf;
			zend_fcall_info_cache lf_cache;
		} extract;
	} fci;
	zval* user_data;
}local_cb_data;

static void init_scan_cbs(local_cb_data* lcbd)
{
	lcbd->fci.scan.ef = empty_fcall_info;
	lcbd->fci.scan.ef_cache = empty_fcall_info_cache;
	lcbd->fci.scan.lf = empty_fcall_info;
	lcbd->fci.scan.lf_cache = empty_fcall_info_cache;
	lcbd->fci.scan.al = empty_fcall_info;
	lcbd->fci.scan.al_cache = empty_fcall_info_cache;
	lcbd->user_data = 0;
}

static long local_callback(zend_fcall_info fci, zend_fcall_info_cache fci_cache, zval **args[], long argc)
{
	zval* retval_ptr;
	if (ZEND_FCI_INITIALIZED(fci)) {
		fci.retval_ptr_ptr = &retval_ptr;
		fci.param_count = argc;
		fci.params = args;
		fci.no_separation = 0;
		if (zend_call_function(&fci, &fci_cache TSRMLS_CC) != SUCCESS || !retval_ptr) {
			return -1;
		} else {
			long retval;
			convert_to_long_ex(&retval_ptr);
			retval = Z_LVAL_P(retval_ptr);
			zval_ptr_dtor(&retval_ptr);
			return retval;
		}
	}
	return -1;
}

static LSCT __stdcall local_scan_enter_file(const char* file_name, uint32_t depth, void* user_data)
{
	local_cb_data* lcbd = (local_cb_data*)user_data;
	zval *zname, *zdepth, *zdata;
	zval **args[3];
	EXT_SET_STRING_PARAM(zname, file_name, args[0]);
	EXT_SET_LONG_PARAM(zdepth, depth, args[1]);
	EXT_SET_ZVAL_PARAM(zdata, lcbd->user_data, args[2]);
	long l = local_callback(lcbd->fci.scan.ef, lcbd->fci.scan.ef_cache, args, 3);
	EXT_DESTORY_PARAM(args, 3);
	if (LSCT_CONTINUE != l || LSCT_ABORT != l)
		return LSCT_CONTINUE;
	return l;
}

static void __stdcall local_scan_leave_file(const char* file_name, uint32_t depth, void* user_data, LXLVT l)
{
	local_cb_data* lcbd = (local_cb_data*)user_data;
	zval *zname, *zdepth, *zdata, *zl;
	zval **args[4];
	EXT_SET_STRING_PARAM(zname, file_name, args[0]);
	EXT_SET_LONG_PARAM(zdepth, depth, args[1]);
	EXT_SET_ZVAL_PARAM(zdata, lcbd->user_data, args[2]);
	EXT_SET_LONG_PARAM(zl, l, args[3]);
	local_callback(lcbd->fci.scan.lf, lcbd->fci.scan.lf_cache, args, 4);
	EXT_DESTORY_PARAM(args, 4);
}

static LSCT __stdcall local_scan_alarm(const char* file_name, lame_scan_result* result, void* user_data)
{
	local_cb_data* lcbd = (local_cb_data*)user_data;
	zval *zname, *zdr, *zdata;
	zval **args[3];
	EXT_SET_STRING_PARAM(zname, file_name, args[0]);
	EXT_SET_ZOBJ_PARAM(zdr, result, args[1]);
	EXT_SET_ZVAL_PARAM(zdata, lcbd->user_data, args[2]);
	long l = local_callback(lcbd->fci.scan.al, lcbd->fci.scan.al_cache, args, 3);
	EXT_COPY_ZVAL(lcbd->user_data, zdata);
	EXT_DESTORY_PARAM(args, 3);
	if (LSCT_CONTINUE != l || LSCT_ABORT != l)
		return LSCT_CONTINUE;
	return l;
}

/* {{{ proto long
	lame_scan_file_with_callback(long lame, string fname, function ef, function lf, function a, zval user_data)
 */
PHP_FUNCTION(lame_scan_file_with_callback)
{
	zval *user_data = NULL;
	char *filename = NULL;
	long lame, filename_len;
	local_cb_data lcbd;
	init_scan_cbs(&lcbd);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsf!f!f!z!", &lame, &filename, &filename_len, &lcbd.fci.scan.ef,
		&lcbd.fci.scan.ef_cache, &lcbd.fci.scan.lf, &lcbd.fci.scan.lf_cache, &lcbd.fci.scan.al,
		&lcbd.fci.scan.al_cache, &user_data) == FAILURE) {
		return;
	}
	if(!(PZVAL_IS_REF(user_data))) {
		return;
	}
	lcbd.user_data = user_data;
	lame_scan_feedback lscb;
	lscb.enter_file = local_scan_enter_file;
	lscb.leave_file = local_scan_leave_file;
	lscb.alarm = local_scan_alarm;
	long result = lame_scan_file_with_callback(lame, filename, &lscb, &lcbd);
	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto long
	lame_scan_mem_with_callback(long lame, string data, function ef, function lf, function a, zval user_data)
 */
PHP_FUNCTION(lame_scan_mem_with_callback)
{
	zval *user_data = NULL;
	char *data = NULL;
	long lame, data_len;
	local_cb_data lcbd;
	init_scan_cbs(&lcbd);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lsf!f!f!z!", &lame, &data, &data_len, &lcbd.fci.scan.ef,
		&lcbd.fci.scan.ef_cache, &lcbd.fci.scan.lf, &lcbd.fci.scan.lf_cache, &lcbd.fci.scan.al,
		&lcbd.fci.scan.al_cache, &user_data) == FAILURE) {
		return;
	}
	if(!(PZVAL_IS_REF(user_data))) {
		return;
	}
	lcbd.user_data = user_data;
	lame_scan_feedback lscb;
	lscb.enter_file = local_scan_enter_file;
	lscb.leave_file = local_scan_leave_file;
	lscb.alarm = local_scan_alarm;
	long result = lame_scan_mem_with_callback(lame, data, data_len, &lscb, &lcbd);
	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto Array
	lame_get_version()
 */
PHP_FUNCTION(lame_get_version)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	lame_info info;
	if(0 == lame_get_version(&info)) {
		EXT_OBJECT_INIT;
		EXT_OBJECT_ADD_STRING("engv", info.engv);
		EXT_OBJECT_ADD_STRING("vdbv", info.vdbv);
		EXT_OBJECT_RETURN;
	}
}
/* }}} */

/* {{{ proto Array
	lame_get_licence_info()
 */
PHP_FUNCTION(lame_get_licence_info)
{
	if (zend_parse_parameters_none() == FAILURE) {
		return;
	}
	rx_licence_info info;
	if (0 == lame_get_licence_info(&info)) {
		EXT_OBJECT_INIT;
		EXT_OBJECT_ADD_STRING("Version", info.Version);
		EXT_OBJECT_ADD_STRING("Owner", info.Owner);
		EXT_OBJECT_ADD_STRING("Date", info.Date);
		EXT_OBJECT_ADD_STRING("Authm", info.Authm);
		EXT_OBJECT_ADD_STRING("Data", info.Data);
		EXT_OBJECT_RETURN;
	}
}
/* }}} */

static LXCT __stdcall local_extract_enter_file(const char* file_name, const char* fmt,
	uint32_t depth, uint64_t flag, void* handle, void* user_data)
{
	local_cb_data* lcbd = (local_cb_data*)user_data;
	zval *zname, *zfmt, *zdepth, *zflag, *zhandle, *zdata;
	zval **args[6];
	EXT_SET_STRING_PARAM(zname, file_name, args[0]);
	EXT_SET_STRING_PARAM(zfmt, fmt, args[1]);
	EXT_SET_LONG_PARAM(zdepth, depth, args[2]);
	EXT_SET_LONG_PARAM(zflag, flag, args[3]);
	EXT_SET_LONG_PARAM(zhandle, (long)handle, args[4]);
	EXT_SET_ZVAL_PARAM(zdata, lcbd->user_data, args[5]);
	long l = local_callback(lcbd->fci.extract.ef, lcbd->fci.extract.ef_cache, args, 6);
	EXT_DESTORY_PARAM(args, 6);
	if (LCT_DO_EXTRACT != l || LCT_CONTINUE != l || LCT_ABORT != l)
		return LCT_DO_EXTRACT;
	return l;
}

static void __stdcall local_extract_leave_file(const char* file_name, const char* fmt, uint32_t depth,
	uint64_t flag, void* handle, void* user_data, LXLVT lv)
{
	local_cb_data* lcbd = (local_cb_data*)user_data;
	zval *zname, *zfmt, *zdepth, *zflag, *zhandle, *zdata, *zlv;
	zval **args[7];
	EXT_SET_STRING_PARAM(zname, file_name, args[0]);
	EXT_SET_STRING_PARAM(zfmt, fmt, args[1]);
	EXT_SET_LONG_PARAM(zdepth, depth, args[2]);
	EXT_SET_LONG_PARAM(zflag, flag, args[3]);
	EXT_SET_LONG_PARAM(zhandle, (long)handle, args[4]);
	EXT_SET_ZVAL_PARAM(zdata, lcbd->user_data, args[5]);
	EXT_SET_LONG_PARAM(zlv, lv, args[6]);
	local_callback(lcbd->fci.extract.lf, lcbd->fci.extract.lf_cache, args, 7);
	EXT_DESTORY_PARAM(args, 7);
}

/* {{{ proto long
	lame_extract_file(long lame, string fname, string password, long* cb, long* userdata)
 */
PHP_FUNCTION(lame_extract_file)
{
	zval *user_data = NULL;
	char *filename = NULL, *passwd = NULL;
	long lame, filename_len, passwd_len;
	local_cb_data lcbd;
	init_scan_cbs(&lcbd);
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lss!f!f!z!", &lame, &filename, &filename_len, &passwd, &passwd_len,
		&lcbd.fci.extract.ef, &lcbd.fci.extract.ef_cache, &lcbd.fci.extract.lf, &lcbd.fci.extract.lf_cache, &user_data) == FAILURE) {
		return;
	}
	if(!(PZVAL_IS_REF(user_data))) {
		return;
	}
	lcbd.user_data = user_data;
	lame_extract_feedback lscb;
	lscb.enter_file = local_extract_enter_file;
	lscb.leave_file = local_extract_leave_file;
	long result = lame_extract_file(lame, filename, passwd, &lscb, &lcbd);
	RETURN_LONG(result);
}
/* }}} */

/* {{{ proto long
	lame_file_get_size(long handle)
 */
PHP_FUNCTION(lame_file_get_size)
{
	long handle;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &handle) == FAILURE) {
		return;
	}
	long size = lame_file_get_size(handle);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto bool
	lame_file_seek(long handle, long offset, long method)
 */
PHP_FUNCTION(lame_file_seek)
{
	long handle, offset, method;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "lll", &handle, &offset, &method) == FAILURE) {
		return;
	}
	long result = lame_file_seek(handle, offset, method);
	if (0 == result) {
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto long
	lame_file_tell(long handle)
 */
PHP_FUNCTION(lame_file_tell)
{
	long handle;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "l", &handle) == FAILURE) {
		return;
	}
	long size = lame_file_tell(handle);
	RETURN_LONG(size);
}
/* }}} */

/* {{{ proto string
	lame_file_read(long handle, long size) */
PHP_FUNCTION(lame_file_read)
{
	long handle, size;
	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "ll", &handle, &size) == FAILURE) {
		return;
	}
	void *buffer = emalloc(size);
	long length = lame_file_read(handle, buffer, size);
	RETURN_STRINGL(buffer, length, 0);
}
/* }}} */


/* {{{ php_plame_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_plame_init_globals(zend_plame_globals *plame_globals)
{
	plame_globals->global_value = 0;
	plame_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(plame)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(plame)
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
PHP_RINIT_FUNCTION(plame)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(plame)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(plame)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "plame support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

ZEND_BEGIN_ARG_INFO_EX(lame_scan_file_with_callback_arginfo, 0, 0, -1)
	ZEND_ARG_INFO(0, lame)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, enter_file)
	ZEND_ARG_INFO(0, leave_file)
	ZEND_ARG_INFO(0, alarm)
	ZEND_ARG_INFO(1, user_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lame_scan_mem_with_callback_arginfo, 0, 0, -1)
	ZEND_ARG_INFO(0, lame)
	ZEND_ARG_INFO(0, contents)
	ZEND_ARG_INFO(0, enter_file)
	ZEND_ARG_INFO(0, leave_file)
	ZEND_ARG_INFO(0, alarm)
	ZEND_ARG_INFO(1, user_data)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(lame_extract_file_arginfo, 0, 0, -1)
	ZEND_ARG_INFO(0, lame)
	ZEND_ARG_INFO(0, filename)
	ZEND_ARG_INFO(0, passwd)
	ZEND_ARG_INFO(0, enter_file)
	ZEND_ARG_INFO(0, leave_file)
	ZEND_ARG_INFO(1, user_data)
ZEND_END_ARG_INFO()

/* {{{ plame_functions[]
 *
 * Every user visible function must have an entry in plame_functions[].
 */
const zend_function_entry plame_functions[] = {
	PHP_FE(confirm_plame_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(lame_open_vdb,	NULL)
	PHP_FE(lame_close_vdb,	NULL)
	PHP_FE(lame_create,	NULL)
	PHP_FE(lame_destroy,	NULL)
	PHP_FE(lame_fork,	NULL)
	PHP_FE(lame_param_set,	NULL)
	PHP_FE(lame_init,	NULL)
	PHP_FE(lame_scan_file,	NULL)
	PHP_FE(lame_scan_mem,	NULL)
	PHP_FE(lame_scan_file_with_callback,	lame_scan_file_with_callback_arginfo)
	PHP_FE(lame_scan_mem_with_callback,	lame_scan_mem_with_callback_arginfo)
	PHP_FE(lame_get_version,	NULL)
	PHP_FE(lame_get_licence_info,	NULL)
	PHP_FE(lame_extract_file,	lame_extract_file_arginfo)
	PHP_FE(lame_file_get_size,	NULL)
	PHP_FE(lame_file_seek,	NULL)
	PHP_FE(lame_file_tell,	NULL)
	PHP_FE(lame_file_read,	NULL)
	PHP_FE_END	/* Must be the last line in plame_functions[] */
};
/* }}} */

/* {{{ plame_module_entry
 */
zend_module_entry plame_module_entry = {
	STANDARD_MODULE_HEADER,
	"plame",
	plame_functions,
	PHP_MINIT(plame),
	PHP_MSHUTDOWN(plame),
	PHP_RINIT(plame),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(plame),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(plame),
	PHP_PLAME_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_PLAME
ZEND_GET_MODULE(plame)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
