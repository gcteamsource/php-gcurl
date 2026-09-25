/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: 8e981b41236e35883f57b46c09bb28445219e48a */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gcurl_version, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, age, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gcurl_init, 0, 0, GCurlHandle, MAY_BE_FALSE)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, url, IS_STRING, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_impersonate, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, target, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, default_headers, _IS_BOOL, 0, "true")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_setopt_array, 0, 2, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
	ZEND_ARG_TYPE_INFO(0, options, IS_ARRAY, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gcurl_exec, 0, 1, MAY_BE_STRING|MAY_BE_BOOL)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_getinfo, 0, 1, IS_MIXED, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, option, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_error, 0, 1, IS_STRING, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_strerror, 0, 1, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, error_code, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_reset, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

#define arginfo_gcurl_close arginfo_gcurl_reset

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_TYPE_MASK_EX(arginfo_gcurl_copy_handle, 0, 1, GCurlHandle, MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gcurl_multi_init, 0, 0, GCurlMultiHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_add_handle, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

#define arginfo_gcurl_multi_remove_handle arginfo_gcurl_multi_add_handle

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_exec, 0, 2, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
	ZEND_ARG_TYPE_INFO(1, still_running, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_select, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_DOUBLE, 0, "1.0")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_gcurl_multi_info_read, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(1, queued_messages, IS_LONG, 1, "null")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_getcontent, 0, 1, IS_STRING, 1)
	ZEND_ARG_OBJ_INFO(0, handle, GCurlHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_errno, 0, 1, IS_LONG, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
ZEND_END_ARG_INFO()

#define arginfo_gcurl_multi_strerror arginfo_gcurl_strerror

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_multi_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, multi_handle, GCurlMultiHandle, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_OBJ_INFO_EX(arginfo_gcurl_share_init, 0, 0, GCurlShareHandle, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_share_setopt, 0, 3, _IS_BOOL, 0)
	ZEND_ARG_OBJ_INFO(0, share_handle, GCurlShareHandle, 0)
	ZEND_ARG_TYPE_INFO(0, option, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, value, IS_MIXED, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_gcurl_share_close, 0, 1, IS_VOID, 0)
	ZEND_ARG_OBJ_INFO(0, share_handle, GCurlShareHandle, 0)
ZEND_END_ARG_INFO()

ZEND_FUNCTION(gcurl_version);
ZEND_FUNCTION(gcurl_init);
ZEND_FUNCTION(gcurl_impersonate);
ZEND_FUNCTION(gcurl_setopt);
ZEND_FUNCTION(gcurl_setopt_array);
ZEND_FUNCTION(gcurl_exec);
ZEND_FUNCTION(gcurl_getinfo);
ZEND_FUNCTION(gcurl_errno);
ZEND_FUNCTION(gcurl_error);
ZEND_FUNCTION(gcurl_strerror);
ZEND_FUNCTION(gcurl_reset);
ZEND_FUNCTION(gcurl_close);
ZEND_FUNCTION(gcurl_copy_handle);
ZEND_FUNCTION(gcurl_multi_init);
ZEND_FUNCTION(gcurl_multi_add_handle);
ZEND_FUNCTION(gcurl_multi_remove_handle);
ZEND_FUNCTION(gcurl_multi_exec);
ZEND_FUNCTION(gcurl_multi_select);
ZEND_FUNCTION(gcurl_multi_info_read);
ZEND_FUNCTION(gcurl_multi_getcontent);
ZEND_FUNCTION(gcurl_multi_errno);
ZEND_FUNCTION(gcurl_multi_strerror);
ZEND_FUNCTION(gcurl_multi_close);
ZEND_FUNCTION(gcurl_multi_setopt);
ZEND_FUNCTION(gcurl_share_init);
ZEND_FUNCTION(gcurl_share_setopt);
ZEND_FUNCTION(gcurl_share_close);

static const zend_function_entry ext_functions[] = {
	ZEND_FE(gcurl_version, arginfo_gcurl_version)
	ZEND_FE(gcurl_init, arginfo_gcurl_init)
	ZEND_FE(gcurl_impersonate, arginfo_gcurl_impersonate)
	ZEND_FE(gcurl_setopt, arginfo_gcurl_setopt)
	ZEND_FE(gcurl_setopt_array, arginfo_gcurl_setopt_array)
	ZEND_FE(gcurl_exec, arginfo_gcurl_exec)
	ZEND_FE(gcurl_getinfo, arginfo_gcurl_getinfo)
	ZEND_FE(gcurl_errno, arginfo_gcurl_errno)
	ZEND_FE(gcurl_error, arginfo_gcurl_error)
	ZEND_FE(gcurl_strerror, arginfo_gcurl_strerror)
	ZEND_FE(gcurl_reset, arginfo_gcurl_reset)
	ZEND_FE(gcurl_close, arginfo_gcurl_close)
	ZEND_FE(gcurl_copy_handle, arginfo_gcurl_copy_handle)
	ZEND_FE(gcurl_multi_init, arginfo_gcurl_multi_init)
	ZEND_FE(gcurl_multi_add_handle, arginfo_gcurl_multi_add_handle)
	ZEND_FE(gcurl_multi_remove_handle, arginfo_gcurl_multi_remove_handle)
	ZEND_FE(gcurl_multi_exec, arginfo_gcurl_multi_exec)
	ZEND_FE(gcurl_multi_select, arginfo_gcurl_multi_select)
	ZEND_FE(gcurl_multi_info_read, arginfo_gcurl_multi_info_read)
	ZEND_FE(gcurl_multi_getcontent, arginfo_gcurl_multi_getcontent)
	ZEND_FE(gcurl_multi_errno, arginfo_gcurl_multi_errno)
	ZEND_FE(gcurl_multi_strerror, arginfo_gcurl_multi_strerror)
	ZEND_FE(gcurl_multi_close, arginfo_gcurl_multi_close)
	ZEND_FE(gcurl_multi_setopt, arginfo_gcurl_multi_setopt)
	ZEND_FE(gcurl_share_init, arginfo_gcurl_share_init)
	ZEND_FE(gcurl_share_setopt, arginfo_gcurl_share_setopt)
	ZEND_FE(gcurl_share_close, arginfo_gcurl_share_close)
	ZEND_FE_END
};

static zend_class_entry *register_class_GCurlHandle(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GCurlHandle", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_GCurlMultiHandle(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GCurlMultiHandle", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}

static zend_class_entry *register_class_GCurlShareHandle(void)
{
	zend_class_entry ce, *class_entry;

	INIT_CLASS_ENTRY(ce, "GCurlShareHandle", NULL);
	class_entry = zend_register_internal_class_with_flags(&ce, NULL, ZEND_ACC_FINAL|ZEND_ACC_NOT_SERIALIZABLE);

	return class_entry;
}
