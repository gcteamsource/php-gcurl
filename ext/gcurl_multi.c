#include "php.h"
#include "php_gcurl.h"
#include "gcurl_multi.h"

zend_object *gcurl_multi_new(zend_class_entry *ce)
{
    php_gcurl_multi *mh = ecalloc(1, sizeof(php_gcurl_multi) + zend_object_properties_size(ce));

    zend_object_std_init(&mh->std, ce);
    object_properties_init(&mh->std, ce);
    mh->std.handlers = &gcurl_multi_handlers;

    mh->multi = curl_multi_init();
    if (!mh->multi) {
        zend_throw_error(NULL, "gcurl: Failed to initialize curl multi handle");
        return &mh->std;
    }

    zend_hash_init(&mh->easyh, 8, NULL, NULL, 0);

    return &mh->std;
}

void gcurl_multi_free_obj(zend_object *object)
{
    php_gcurl_multi *mh = gcurl_multi_from_obj(object);

    if (mh->multi) {
        zval *entry;
        ZEND_HASH_FOREACH_VAL(&mh->easyh, entry) {
            php_gcurl_handle *ch = Z_GCURL_HANDLE_P(entry);
            if (ch && ch->cp) {
                curl_multi_remove_handle(mh->multi, ch->cp);
            }
        } ZEND_HASH_FOREACH_END();

        curl_multi_cleanup(mh->multi);
        mh->multi = NULL;
    }

    zend_hash_destroy(&mh->easyh);
    zend_object_std_dtor(&mh->std);
}

PHP_FUNCTION(gcurl_multi_init)
{
    ZEND_PARSE_PARAMETERS_NONE();

    object_init_ex(return_value, gcurl_multi_handle_ce);
    php_gcurl_multi *mh = Z_GCURL_MULTI_P(return_value);
    if (!mh->multi) {
        RETURN_FALSE;
    }
}

PHP_FUNCTION(gcurl_multi_add_handle)
{
    zval *z_mh, *z_ch;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
        Z_PARAM_OBJECT_OF_CLASS(z_ch, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi  *mh = Z_GCURL_MULTI_P(z_mh);
    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(z_ch);

    if (!mh->multi || !ch->cp) {
        RETURN_LONG(CURLM_BAD_HANDLE);
    }

    if (zend_hash_index_exists(&mh->easyh, (zend_ulong)(uintptr_t)ch->cp)) {
        RETURN_LONG(CURLM_ADDED_ALREADY);
    }

    CURLMcode ret = curl_multi_add_handle(mh->multi, ch->cp);
    if (ret == CURLM_OK) {
        Z_ADDREF_P(z_ch);
        zend_hash_index_add(&mh->easyh, (zend_ulong)(uintptr_t)ch->cp, z_ch);
    }

    RETURN_LONG(ret);
}

PHP_FUNCTION(gcurl_multi_remove_handle)
{
    zval *z_mh, *z_ch;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
        Z_PARAM_OBJECT_OF_CLASS(z_ch, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi  *mh = Z_GCURL_MULTI_P(z_mh);
    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(z_ch);

    if (!mh->multi || !ch->cp) {
        RETURN_LONG(CURLM_BAD_HANDLE);
    }

    CURLMcode ret = curl_multi_remove_handle(mh->multi, ch->cp);
    if (ret == CURLM_OK) {
        zval *entry = zend_hash_index_find(&mh->easyh, (zend_ulong)(uintptr_t)ch->cp);
        if (entry) {
            zval_ptr_dtor(entry);
            zend_hash_index_del(&mh->easyh, (zend_ulong)(uintptr_t)ch->cp);
        }
    }

    RETURN_LONG(ret);
}

PHP_FUNCTION(gcurl_multi_exec)
{
    zval *z_mh;
    zval *z_running;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
        Z_PARAM_ZVAL(z_running)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi *mh = Z_GCURL_MULTI_P(z_mh);
    if (!mh->multi) {
        RETURN_LONG(CURLM_BAD_HANDLE);
    }

    int still_running = 0;
    CURLMcode ret = curl_multi_perform(mh->multi, &still_running);

    ZEND_TRY_ASSIGN_REF_LONG(z_running, still_running);
    RETURN_LONG(ret);
}

PHP_FUNCTION(gcurl_multi_select)
{
    zval   *z_mh;
    double  timeout = 1.0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_DOUBLE(timeout)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi *mh = Z_GCURL_MULTI_P(z_mh);
    if (!mh->multi) {
        RETURN_LONG(-1);
    }

    int numfds = 0;
    CURLMcode ret = curl_multi_wait(mh->multi, NULL, 0, (int)(timeout * 1000), &numfds);
    if (ret != CURLM_OK) {
        RETURN_LONG(-1);
    }

    RETURN_LONG(numfds);
}

PHP_FUNCTION(gcurl_multi_info_read)
{
    zval *z_mh;
    zval *z_msgs = NULL;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_ZVAL(z_msgs)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi *mh = Z_GCURL_MULTI_P(z_mh);
    if (!mh->multi) {
        RETURN_FALSE;
    }

    int msgs_in_queue = 0;
    CURLMsg *msg = curl_multi_info_read(mh->multi, &msgs_in_queue);

    if (z_msgs) {
        ZEND_TRY_ASSIGN_REF_LONG(z_msgs, msgs_in_queue);
    }

    if (!msg) {
        RETURN_FALSE;
    }

    array_init(return_value);
    add_assoc_long(return_value, "msg", msg->msg);
    add_assoc_long(return_value, "result", msg->data.result);

    zval *z_ch = zend_hash_index_find(&mh->easyh, (zend_ulong)(uintptr_t)msg->easy_handle);
    if (z_ch) {
        Z_ADDREF_P(z_ch);
        add_assoc_zval(return_value, "handle", z_ch);
    } else {
        add_assoc_null(return_value, "handle");
    }
}

PHP_FUNCTION(gcurl_multi_getcontent)
{
    zval *z_ch;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(z_ch, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(z_ch);

    if (ch->handlers.write_method == PHP_GCURL_RETURN && ch->write_str.s) {
        smart_str_0(&ch->write_str);
        RETURN_STR_COPY(ch->write_str.s);
    }

    RETURN_NULL();
}

PHP_FUNCTION(gcurl_multi_errno)
{
    zval *z_mh;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    RETURN_LONG(0);
}

PHP_FUNCTION(gcurl_multi_strerror)
{
    zend_long error_code;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(error_code)
    ZEND_PARSE_PARAMETERS_END();

    const char *str = curl_multi_strerror((CURLMcode)error_code);
    if (str) {
        RETURN_STRING(str);
    }
    RETURN_NULL();
}

PHP_FUNCTION(gcurl_multi_close)
{
    zval *z_mh;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi *mh = Z_GCURL_MULTI_P(z_mh);
    if (mh->multi) {
        zval *entry;
        ZEND_HASH_FOREACH_VAL(&mh->easyh, entry) {
            php_gcurl_handle *ch = Z_GCURL_HANDLE_P(entry);
            if (ch && ch->cp) {
                curl_multi_remove_handle(mh->multi, ch->cp);
            }
        } ZEND_HASH_FOREACH_END();

        curl_multi_cleanup(mh->multi);
        mh->multi = NULL;
    }
}

PHP_FUNCTION(gcurl_multi_setopt)
{
    zval      *z_mh, *z_val;
    zend_long  option;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(z_mh, gcurl_multi_handle_ce)
        Z_PARAM_LONG(option)
        Z_PARAM_ZVAL(z_val)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_multi *mh = Z_GCURL_MULTI_P(z_mh);
    if (!mh->multi) {
        RETURN_FALSE;
    }

    CURLMcode ret = curl_multi_setopt(mh->multi, option, zval_get_long(z_val));
    RETURN_BOOL(ret == CURLM_OK);
}
