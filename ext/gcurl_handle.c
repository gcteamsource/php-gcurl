#include "php.h"
#include "php_gcurl.h"
#include "gcurl_handle.h"

static size_t gcurl_write_callback(char *data, size_t size, size_t nmemb, void *ctx)
{
    php_gcurl_handle *ch = (php_gcurl_handle *)ctx;
    size_t length = size * nmemb;

    if (ch->in_callback) {
        return length;
    }

    switch (ch->handlers.write_method) {
        case PHP_GCURL_RETURN:
            smart_str_appendl(&ch->write_str, data, length);
            break;

        case PHP_GCURL_STDOUT:
            PHPWRITE(data, length);
            break;

        case PHP_GCURL_FILE: {
            php_stream *stream;
            stream = (php_stream *)zend_fetch_resource_ex(&ch->handlers.write, NULL, php_file_le_stream());
            if (stream) {
                length = php_stream_write(stream, data, length);
            }
            break;
        }

        case PHP_GCURL_USER: {
            zval args[2], retval;
            ZVAL_OBJ(&args[0], &ch->std);
            Z_ADDREF(args[0]);
            ZVAL_STRINGL(&args[1], data, length);

            ch->in_callback = true;
            if (call_user_function(NULL, NULL, &ch->handlers.write, &retval, 2, args) == SUCCESS && !EG(exception)) {
                if (Z_TYPE(retval) == IS_LONG) {
                    length = (size_t)Z_LVAL(retval);
                }
            }
            ch->in_callback = false;

            zval_ptr_dtor(&args[0]);
            zval_ptr_dtor(&args[1]);
            zval_ptr_dtor(&retval);
            break;
        }

        case PHP_GCURL_IGNORE:
        default:
            break;
    }

    return length;
}

static size_t gcurl_header_callback(char *data, size_t size, size_t nmemb, void *ctx)
{
    php_gcurl_handle *ch = (php_gcurl_handle *)ctx;
    size_t length = size * nmemb;

    switch (ch->handlers.header_method) {
        case PHP_GCURL_RETURN:
            smart_str_appendl(&ch->header_str, data, length);
            break;

        case PHP_GCURL_USER: {
            zval args[2], retval;
            ZVAL_OBJ(&args[0], &ch->std);
            Z_ADDREF(args[0]);
            ZVAL_STRINGL(&args[1], data, length);

            ch->in_callback = true;
            call_user_function(NULL, NULL, &ch->handlers.header, &retval, 2, args);
            ch->in_callback = false;

            zval_ptr_dtor(&args[0]);
            zval_ptr_dtor(&args[1]);
            zval_ptr_dtor(&retval);
            break;
        }

        default:
            break;
    }

    return length;
}

zend_object *gcurl_handle_new(zend_class_entry *ce)
{
    php_gcurl_handle *ch = ecalloc(1, sizeof(php_gcurl_handle) + zend_object_properties_size(ce));

    zend_object_std_init(&ch->std, ce);
    object_properties_init(&ch->std, ce);
    ch->std.handlers = &gcurl_handle_handlers;

    ch->cp = curl_easy_init();
    if (!ch->cp) {
        zend_throw_error(NULL, "gcurl: Failed to initialize curl easy handle");
        return &ch->std;
    }

    ch->handlers.write_method  = PHP_GCURL_STDOUT;
    ch->handlers.read_method   = PHP_GCURL_STDOUT;
    ch->handlers.header_method = PHP_GCURL_IGNORE;

    curl_easy_setopt(ch->cp, CURLOPT_ERRORBUFFER, ch->err.str);
    curl_easy_setopt(ch->cp, CURLOPT_WRITEFUNCTION, gcurl_write_callback);
    curl_easy_setopt(ch->cp, CURLOPT_WRITEDATA, (void *)ch);
    curl_easy_setopt(ch->cp, CURLOPT_HEADERFUNCTION, gcurl_header_callback);
    curl_easy_setopt(ch->cp, CURLOPT_HEADERDATA, (void *)ch);

    ZVAL_UNDEF(&ch->postfields);
    ZVAL_UNDEF(&ch->handlers.write);
    ZVAL_UNDEF(&ch->handlers.read);
    ZVAL_UNDEF(&ch->handlers.header);
    ZVAL_UNDEF(&ch->handlers.progress);
    ZVAL_UNDEF(&ch->handlers.std_err);

    return &ch->std;
}

void gcurl_handle_free_obj(zend_object *object)
{
    php_gcurl_handle *ch = gcurl_handle_from_obj(object);

    if (ch->cp) {
        curl_easy_cleanup(ch->cp);
        ch->cp = NULL;
    }

    if (ch->header) {
        curl_slist_free_all(ch->header);
        ch->header = NULL;
    }
    if (ch->postquote) {
        curl_slist_free_all(ch->postquote);
        ch->postquote = NULL;
    }
    if (ch->resolve) {
        curl_slist_free_all(ch->resolve);
        ch->resolve = NULL;
    }
    if (ch->connect_to) {
        curl_slist_free_all(ch->connect_to);
        ch->connect_to = NULL;
    }

    zval_ptr_dtor(&ch->postfields);
    zval_ptr_dtor(&ch->handlers.write);
    zval_ptr_dtor(&ch->handlers.read);
    zval_ptr_dtor(&ch->handlers.header);
    zval_ptr_dtor(&ch->handlers.progress);
    zval_ptr_dtor(&ch->handlers.std_err);

    smart_str_free(&ch->write_str);
    smart_str_free(&ch->header_str);

    zend_object_std_dtor(&ch->std);
}

PHP_FUNCTION(gcurl_init)
{
    zend_string *url = NULL;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_STR_OR_NULL(url)
    ZEND_PARSE_PARAMETERS_END();

    object_init_ex(return_value, gcurl_handle_ce);
    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(return_value);

    if (!ch->cp) {
        RETURN_FALSE;
    }

    if (url) {
        curl_easy_setopt(ch->cp, CURLOPT_URL, ZSTR_VAL(url));
    }
}

PHP_FUNCTION(gcurl_impersonate)
{
    zval        *zid;
    zend_string *target;
    bool         default_headers = 1;

    ZEND_PARSE_PARAMETERS_START(2, 3)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
        Z_PARAM_STR(target)
        Z_PARAM_OPTIONAL
        Z_PARAM_BOOL(default_headers)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    if (UNEXPECTED(!ch->cp)) {
        php_error_docref(NULL, E_WARNING, "Invalid gcurl handle");
        RETURN_FALSE;
    }

    const char *resolved_target = ZSTR_VAL(target);
    if (strcmp(resolved_target, "chrome") == 0) {
        resolved_target = "chrome124";
    } else if (strcmp(resolved_target, "firefox") == 0) {
        resolved_target = "firefox133";
    } else if (strcmp(resolved_target, "safari") == 0) {
        resolved_target = "safari180";
    } else if (strcmp(resolved_target, "edge") == 0) {
        resolved_target = "edge101";
    }

    CURLcode ret = curl_easy_impersonate(ch->cp, resolved_target, (int)default_headers);
    SAVE_GCURL_ERROR(ch, ret);

    if (ret != CURLE_OK) {
        php_error_docref(NULL, E_WARNING, "gcurl_impersonate(): target '%s' failed: %s",
                         ZSTR_VAL(target), curl_easy_strerror(ret));
        RETURN_FALSE;
    }

    /* Reset callbacks in case impersonate reset them */
    curl_easy_setopt(ch->cp, CURLOPT_WRITEFUNCTION, gcurl_write_callback);
    curl_easy_setopt(ch->cp, CURLOPT_WRITEDATA, (void *)ch);
    curl_easy_setopt(ch->cp, CURLOPT_HEADERFUNCTION, gcurl_header_callback);
    curl_easy_setopt(ch->cp, CURLOPT_HEADERDATA, (void *)ch);

    RETURN_TRUE;
}

static zend_result _gcurl_setopt(php_gcurl_handle *ch, zend_long option, zval *zvalue)
{
    CURLcode error = CURLE_OK;

    switch (option) {
        case GCURLOPT_RETURNTRANSFER:
            ch->handlers.write_method = zend_is_true(zvalue) ? PHP_GCURL_RETURN : PHP_GCURL_STDOUT;
            smart_str_free(&ch->write_str);
            return SUCCESS;

        case CURLOPT_URL:
        case CURLOPT_USERAGENT:
        case CURLOPT_REFERER:
        case CURLOPT_COOKIE:
        case CURLOPT_COOKIEFILE:
        case CURLOPT_COOKIEJAR:
        case CURLOPT_CUSTOMREQUEST:
        case CURLOPT_PROXY:
        case CURLOPT_PROXYUSERPWD:
        case CURLOPT_INTERFACE:
        case CURLOPT_ENCODING:
        case CURLOPT_CAINFO:
        case CURLOPT_CAPATH:
        case CURLOPT_SSLCERT:
        case CURLOPT_SSLKEY:
        case CURLOPT_USERPWD:
        case CURLOPT_SSL_CIPHER_LIST: {
            zend_string *str = zval_get_string(zvalue);
            error = curl_easy_setopt(ch->cp, (CURLoption)option, ZSTR_VAL(str));
            zend_string_release(str);
            break;
        }

        case CURLOPT_VERBOSE:
        case CURLOPT_HEADER:
        case CURLOPT_NOPROGRESS:
        case CURLOPT_NOBODY:
        case CURLOPT_FOLLOWLOCATION:
        case CURLOPT_MAXREDIRS:
        case CURLOPT_CONNECTTIMEOUT:
        case CURLOPT_CONNECTTIMEOUT_MS:
        case CURLOPT_TIMEOUT:
        case CURLOPT_TIMEOUT_MS:
        case CURLOPT_PORT:
        case CURLOPT_AUTOREFERER:
        case CURLOPT_HTTPGET:
        case CURLOPT_HTTP_VERSION:
        case CURLOPT_POST:
        case CURLOPT_PUT:
        case CURLOPT_SSL_VERIFYPEER:
        case CURLOPT_SSL_VERIFYHOST:
        case CURLOPT_PROXYPORT:
        case CURLOPT_PROXYTYPE:
        case CURLOPT_HTTPPROXYTUNNEL:
        case CURLOPT_COOKIESESSION:
        case CURLOPT_FAILONERROR: {
            error = curl_easy_setopt(ch->cp, (CURLoption)option, zval_get_long(zvalue));
            break;
        }

        case CURLOPT_HTTPHEADER: {
            if (Z_TYPE_P(zvalue) != IS_ARRAY) {
                zend_type_error("gcurl_setopt(): CURLOPT_HTTPHEADER expects array");
                return FAILURE;
            }
            zval *item;
            struct curl_slist *slist = NULL;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zvalue), item) {
                zend_string *str = zval_get_string(item);
                slist = curl_slist_append(slist, ZSTR_VAL(str));
                zend_string_release(str);
            } ZEND_HASH_FOREACH_END();

            if (ch->header) {
                curl_slist_free_all(ch->header);
            }
            ch->header = slist;
            error = curl_easy_setopt(ch->cp, CURLOPT_HTTPHEADER, slist);
            break;
        }

        case CURLOPT_RESOLVE: {
            if (Z_TYPE_P(zvalue) != IS_ARRAY) {
                zend_type_error("gcurl_setopt(): CURLOPT_RESOLVE expects array");
                return FAILURE;
            }
            zval *item;
            struct curl_slist *slist = NULL;
            ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(zvalue), item) {
                zend_string *str = zval_get_string(item);
                slist = curl_slist_append(slist, ZSTR_VAL(str));
                zend_string_release(str);
            } ZEND_HASH_FOREACH_END();

            if (ch->resolve) {
                curl_slist_free_all(ch->resolve);
            }
            ch->resolve = slist;
            error = curl_easy_setopt(ch->cp, CURLOPT_RESOLVE, slist);
            break;
        }

        case CURLOPT_POSTFIELDS:
            zval_ptr_dtor(&ch->postfields);
            ZVAL_COPY(&ch->postfields, zvalue);
            if (Z_TYPE_P(zvalue) == IS_STRING) {
                curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDSIZE, (curl_off_t)Z_STRLEN_P(zvalue));
                error = curl_easy_setopt(ch->cp, CURLOPT_POSTFIELDS, Z_STRVAL_P(zvalue));
            }
            break;

        case CURLOPT_WRITEFUNCTION:
            if (!zend_is_callable(zvalue, 0, NULL)) {
                zend_type_error("gcurl_setopt(): CURLOPT_WRITEFUNCTION expects callable");
                return FAILURE;
            }
            zval_ptr_dtor(&ch->handlers.write);
            ZVAL_COPY(&ch->handlers.write, zvalue);
            ch->handlers.write_method = PHP_GCURL_USER;
            break;

        case CURLOPT_HEADERFUNCTION:
            if (!zend_is_callable(zvalue, 0, NULL)) {
                zend_type_error("gcurl_setopt(): CURLOPT_HEADERFUNCTION expects callable");
                return FAILURE;
            }
            zval_ptr_dtor(&ch->handlers.header);
            ZVAL_COPY(&ch->handlers.header, zvalue);
            ch->handlers.header_method = PHP_GCURL_USER;
            break;

        case CURLOPT_FILE: {
            php_stream *stream;
            php_stream_from_zval_no_verify(stream, zvalue);
            if (!stream) {
                zend_type_error("gcurl_setopt(): CURLOPT_FILE expects a valid stream resource");
                return FAILURE;
            }
            zval_ptr_dtor(&ch->handlers.write);
            ZVAL_COPY(&ch->handlers.write, zvalue);
            ch->handlers.write_method = PHP_GCURL_FILE;
            break;
        }

        case CURLOPT_SHARE: {
            if (Z_TYPE_P(zvalue) == IS_OBJECT && instanceof_function(Z_OBJCE_P(zvalue), gcurl_share_handle_ce)) {
                php_gcurl_share *sh = Z_GCURL_SHARE_P(zvalue);
                error = curl_easy_setopt(ch->cp, CURLOPT_SHARE, sh->share);
            } else {
                zend_type_error("gcurl_setopt(): CURLOPT_SHARE expects a GCurlShareHandle object");
                return FAILURE;
            }
            break;
        }

        default:
            error = curl_easy_setopt(ch->cp, (CURLoption)option, zval_get_long(zvalue));
            break;
    }

    SAVE_GCURL_ERROR(ch, error);
    return (error == CURLE_OK) ? SUCCESS : FAILURE;
}

PHP_FUNCTION(gcurl_setopt)
{
    zval      *zid, *zvalue;
    zend_long  option;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
        Z_PARAM_LONG(option)
        Z_PARAM_ZVAL(zvalue)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    RETURN_BOOL(_gcurl_setopt(ch, option, zvalue) == SUCCESS);
}

PHP_FUNCTION(gcurl_setopt_array)
{
    zval *zid, *arr;

    ZEND_PARSE_PARAMETERS_START(2, 2)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
        Z_PARAM_ARRAY(arr)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);

    zend_ulong opt_idx;
    zend_string *opt_key;
    zval *val;

    ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(arr), opt_idx, opt_key, val) {
        zend_long option = (zend_long)opt_idx;
        if (opt_key) {
            continue;
        }
        if (_gcurl_setopt(ch, option, val) != SUCCESS) {
            RETURN_FALSE;
        }
    } ZEND_HASH_FOREACH_END();

    RETURN_TRUE;
}

PHP_FUNCTION(gcurl_exec)
{
    zval *zid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    if (UNEXPECTED(!ch->cp)) {
        RETURN_FALSE;
    }

    smart_str_free(&ch->write_str);
    smart_str_free(&ch->header_str);

    CURLcode ret = curl_easy_perform(ch->cp);
    SAVE_GCURL_ERROR(ch, ret);

    if (ret != CURLE_OK) {
        RETURN_FALSE;
    }

    if (ch->handlers.write_method == PHP_GCURL_RETURN) {
        if (ch->write_str.s) {
            smart_str_0(&ch->write_str);
            RETURN_STR_COPY(ch->write_str.s);
        }
        RETURN_EMPTY_STRING();
    }

    RETURN_TRUE;
}

PHP_FUNCTION(gcurl_getinfo)
{
    zval      *zid;
    zend_long  option = 0;

    ZEND_PARSE_PARAMETERS_START(1, 2)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(option)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    if (!ch->cp) {
        RETURN_FALSE;
    }

    if (ZEND_NUM_ARGS() == 2) {
        char      *sval = NULL;
        zend_long  lval = 0;
        double     dval = 0.0;

        switch (option) {
            case CURLINFO_RESPONSE_CODE:
            case CURLINFO_REDIRECT_COUNT:
            case CURLINFO_HEADER_SIZE:
            case CURLINFO_REQUEST_SIZE:
            case CURLINFO_HTTP_VERSION:
            case CURLINFO_LOCAL_PORT:
            case CURLINFO_PRIMARY_PORT:
                curl_easy_getinfo(ch->cp, (CURLINFO)option, &lval);
                RETURN_LONG(lval);

            case CURLINFO_TOTAL_TIME:
            case CURLINFO_NAMELOOKUP_TIME:
            case CURLINFO_CONNECT_TIME:
            case CURLINFO_APPCONNECT_TIME:
            case CURLINFO_PRETRANSFER_TIME:
            case CURLINFO_STARTTRANSFER_TIME:
            case CURLINFO_REDIRECT_TIME:
            case CURLINFO_SIZE_DOWNLOAD_T:
            case CURLINFO_SIZE_UPLOAD_T:
                curl_easy_getinfo(ch->cp, (CURLINFO)option, &dval);
                RETURN_DOUBLE(dval);

            case CURLINFO_EFFECTIVE_URL:
            case CURLINFO_CONTENT_TYPE:
            case CURLINFO_REDIRECT_URL:
            case CURLINFO_PRIMARY_IP:
            case CURLINFO_LOCAL_IP:
            case CURLINFO_SCHEME:
                curl_easy_getinfo(ch->cp, (CURLINFO)option, &sval);
                if (sval) {
                    RETURN_STRING(sval);
                }
                RETURN_NULL();

            default:
                RETURN_FALSE;
        }
    }

    array_init(return_value);

    char      *sval = NULL;
    zend_long  lval = 0;
    double     dval = 0.0;

    #define GCURL_INFO_LONG(key, opt) \
        curl_easy_getinfo(ch->cp, opt, &lval); \
        add_assoc_long(return_value, key, lval);

    #define GCURL_INFO_DOUBLE(key, opt) \
        curl_easy_getinfo(ch->cp, opt, &dval); \
        add_assoc_double(return_value, key, dval);

    #define GCURL_INFO_STRING(key, opt) \
        curl_easy_getinfo(ch->cp, opt, &sval); \
        add_assoc_string(return_value, key, sval ? sval : "");

    GCURL_INFO_STRING("url",                 CURLINFO_EFFECTIVE_URL);
    GCURL_INFO_LONG  ("http_code",           CURLINFO_HTTP_CODE);
    GCURL_INFO_DOUBLE("total_time",          CURLINFO_TOTAL_TIME);
    GCURL_INFO_DOUBLE("namelookup_time",     CURLINFO_NAMELOOKUP_TIME);
    GCURL_INFO_DOUBLE("connect_time",        CURLINFO_CONNECT_TIME);
    GCURL_INFO_DOUBLE("appconnect_time",     CURLINFO_APPCONNECT_TIME);
    GCURL_INFO_DOUBLE("pretransfer_time",    CURLINFO_PRETRANSFER_TIME);
    GCURL_INFO_DOUBLE("starttransfer_time",  CURLINFO_STARTTRANSFER_TIME);
    GCURL_INFO_DOUBLE("redirect_time",       CURLINFO_REDIRECT_TIME);
    GCURL_INFO_LONG  ("redirect_count",      CURLINFO_REDIRECT_COUNT);
    GCURL_INFO_STRING("redirect_url",        CURLINFO_REDIRECT_URL);
    GCURL_INFO_DOUBLE("size_download",       CURLINFO_SIZE_DOWNLOAD_T);
    GCURL_INFO_DOUBLE("size_upload",         CURLINFO_SIZE_UPLOAD_T);
    GCURL_INFO_LONG  ("header_size",         CURLINFO_HEADER_SIZE);
    GCURL_INFO_LONG  ("request_size",        CURLINFO_REQUEST_SIZE);
    GCURL_INFO_STRING("content_type",        CURLINFO_CONTENT_TYPE);
    GCURL_INFO_STRING("primary_ip",          CURLINFO_PRIMARY_IP);
    GCURL_INFO_LONG  ("primary_port",        CURLINFO_PRIMARY_PORT);
    GCURL_INFO_STRING("local_ip",            CURLINFO_LOCAL_IP);
    GCURL_INFO_LONG  ("local_port",          CURLINFO_LOCAL_PORT);
    GCURL_INFO_STRING("scheme",              CURLINFO_SCHEME);
    GCURL_INFO_LONG  ("http_version",        CURLINFO_HTTP_VERSION);

    add_assoc_long(return_value, "errno", ch->err.no);
    add_assoc_string(return_value, "error", ch->err.str[0] ? ch->err.str : "");

    #undef GCURL_INFO_LONG
    #undef GCURL_INFO_DOUBLE
    #undef GCURL_INFO_STRING
}

PHP_FUNCTION(gcurl_errno)
{
    zval *zid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    RETURN_LONG(ch->err.no);
}

PHP_FUNCTION(gcurl_error)
{
    zval *zid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    RETURN_STRING(ch->err.str[0] ? ch->err.str : "");
}

PHP_FUNCTION(gcurl_strerror)
{
    zend_long error_code;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_LONG(error_code)
    ZEND_PARSE_PARAMETERS_END();

    const char *str = curl_easy_strerror((CURLcode)error_code);
    if (str) {
        RETURN_STRING(str);
    }
    RETURN_NULL();
}

PHP_FUNCTION(gcurl_reset)
{
    zval *zid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *ch = Z_GCURL_HANDLE_P(zid);
    if (ch->cp) {
        curl_easy_reset(ch->cp);

        ch->handlers.write_method  = PHP_GCURL_STDOUT;
        ch->handlers.read_method   = PHP_GCURL_STDOUT;
        ch->handlers.header_method = PHP_GCURL_IGNORE;

        curl_easy_setopt(ch->cp, CURLOPT_ERRORBUFFER, ch->err.str);
        curl_easy_setopt(ch->cp, CURLOPT_WRITEFUNCTION, gcurl_write_callback);
        curl_easy_setopt(ch->cp, CURLOPT_WRITEDATA, (void *)ch);
        curl_easy_setopt(ch->cp, CURLOPT_HEADERFUNCTION, gcurl_header_callback);
        curl_easy_setopt(ch->cp, CURLOPT_HEADERDATA, (void *)ch);
    }
}

PHP_FUNCTION(gcurl_close)
{
    zval *zid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    /* Object destruction handles cleanup */
}

PHP_FUNCTION(gcurl_copy_handle)
{
    zval *zid;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(zid, gcurl_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_handle *src = Z_GCURL_HANDLE_P(zid);
    if (!src->cp) {
        RETURN_FALSE;
    }

    object_init_ex(return_value, gcurl_handle_ce);
    php_gcurl_handle *dest = Z_GCURL_HANDLE_P(return_value);

    if (dest->cp) {
        curl_easy_cleanup(dest->cp);
    }
    dest->cp = curl_easy_duphandle(src->cp);
    if (!dest->cp) {
        RETURN_FALSE;
    }

    dest->handlers.write_method = src->handlers.write_method;
    dest->handlers.read_method = src->handlers.read_method;
    dest->handlers.header_method = src->handlers.header_method;

    if (!Z_ISUNDEF(src->handlers.write)) {
        ZVAL_COPY(&dest->handlers.write, &src->handlers.write);
    }
    if (!Z_ISUNDEF(src->handlers.header)) {
        ZVAL_COPY(&dest->handlers.header, &src->handlers.header);
    }

    curl_easy_setopt(dest->cp, CURLOPT_ERRORBUFFER, dest->err.str);
    curl_easy_setopt(dest->cp, CURLOPT_WRITEFUNCTION, gcurl_write_callback);
    curl_easy_setopt(dest->cp, CURLOPT_WRITEDATA, (void *)dest);
    curl_easy_setopt(dest->cp, CURLOPT_HEADERFUNCTION, gcurl_header_callback);
    curl_easy_setopt(dest->cp, CURLOPT_HEADERDATA, (void *)dest);
}
