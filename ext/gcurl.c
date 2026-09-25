#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "php_gcurl.h"
#include "gcurl_handle.h"
#include "gcurl_multi.h"
#include "gcurl_share.h"
#include "gcurl_handle_arginfo.h"

zend_class_entry *gcurl_handle_ce;
zend_class_entry *gcurl_multi_handle_ce;
zend_class_entry *gcurl_share_handle_ce;

zend_object_handlers gcurl_handle_handlers;
zend_object_handlers gcurl_multi_handlers;
zend_object_handlers gcurl_share_handlers;

PHP_FUNCTION(gcurl_version)
{
    zend_long age = CURLVERSION_NOW;

    ZEND_PARSE_PARAMETERS_START(0, 1)
        Z_PARAM_OPTIONAL
        Z_PARAM_LONG(age)
    ZEND_PARSE_PARAMETERS_END();

    curl_version_info_data *d = curl_version_info((CURLversion)age);
    if (!d) {
        RETURN_FALSE;
    }

    array_init(return_value);
    add_assoc_string(return_value, "gcurl_version", PHP_GCURL_VERSION);
    add_assoc_string(return_value, "version", d->version ? d->version : "");
    add_assoc_long(return_value, "version_number", d->version_num);
    add_assoc_string(return_value, "host", d->host ? d->host : "");
    add_assoc_long(return_value, "features", d->features);
    add_assoc_string(return_value, "ssl_version", d->ssl_version ? d->ssl_version : "");
    add_assoc_long(return_value, "ssl_version_num", d->ssl_version_num);
    add_assoc_string(return_value, "libz_version", d->libz_version ? d->libz_version : "");

    if (d->brotli_version) {
        add_assoc_string(return_value, "brotli_version", d->brotli_version);
    }
    if (d->zstd_version) {
        add_assoc_string(return_value, "zstd_version", d->zstd_version);
    }
    if (d->nghttp2_version) {
        add_assoc_string(return_value, "nghttp2_version", d->nghttp2_version);
    }

    zval protocols;
    array_init(&protocols);
    if (d->protocols) {
        const char * const *p = d->protocols;
        while (*p) {
            add_next_index_string(&protocols, *p);
            p++;
        }
    }
    add_assoc_zval(return_value, "protocols", &protocols);
}

PHP_MINIT_FUNCTION(gcurl)
{
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        return FAILURE;
    }

    gcurl_handle_ce = register_class_GCurlHandle();
    gcurl_handle_ce->create_object = gcurl_handle_new;

    memcpy(&gcurl_handle_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    gcurl_handle_handlers.offset    = XtOffsetOf(php_gcurl_handle, std);
    gcurl_handle_handlers.free_obj  = gcurl_handle_free_obj;
    gcurl_handle_handlers.clone_obj = NULL;

    gcurl_multi_handle_ce = register_class_GCurlMultiHandle();
    gcurl_multi_handle_ce->create_object = gcurl_multi_new;

    memcpy(&gcurl_multi_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    gcurl_multi_handlers.offset    = XtOffsetOf(php_gcurl_multi, std);
    gcurl_multi_handlers.free_obj  = gcurl_multi_free_obj;
    gcurl_multi_handlers.clone_obj = NULL;

    gcurl_share_handle_ce = register_class_GCurlShareHandle();
    gcurl_share_handle_ce->create_object = gcurl_share_new;

    memcpy(&gcurl_share_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
    gcurl_share_handlers.offset    = XtOffsetOf(php_gcurl_share, std);
    gcurl_share_handlers.free_obj  = gcurl_share_free_obj;
    gcurl_share_handlers.clone_obj = NULL;

    gcurl_register_constants(module_number);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(gcurl)
{
    curl_global_cleanup();
    return SUCCESS;
}

PHP_MINFO_FUNCTION(gcurl)
{
    curl_version_info_data *d = curl_version_info(CURLVERSION_NOW);

    php_info_print_table_start();
    php_info_print_table_header(2, "gcurl support", "enabled");
    php_info_print_table_row(2, "gcurl extension version", PHP_GCURL_VERSION);
    php_info_print_table_row(2, "libcurl-impersonate version", d->version ? d->version : "unknown");
    php_info_print_table_row(2, "SSL version", d->ssl_version ? d->ssl_version : "unknown");
    if (d->brotli_version) {
        php_info_print_table_row(2, "brotli version", d->brotli_version);
    }
    if (d->zstd_version) {
        php_info_print_table_row(2, "zstd version", d->zstd_version);
    }
    if (d->nghttp2_version) {
        php_info_print_table_row(2, "nghttp2 version", d->nghttp2_version);
    }
    php_info_print_table_end();
}

zend_module_entry gcurl_module_entry = {
    STANDARD_MODULE_HEADER,
    PHP_GCURL_EXTNAME,
    ext_functions,
    PHP_MINIT(gcurl),
    PHP_MSHUTDOWN(gcurl),
    NULL,
    NULL,
    PHP_MINFO(gcurl),
    PHP_GCURL_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_GCURL
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(gcurl)
#endif
