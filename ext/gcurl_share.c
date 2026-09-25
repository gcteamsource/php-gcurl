#include "php.h"
#include "php_gcurl.h"
#include "gcurl_share.h"

zend_object *gcurl_share_new(zend_class_entry *ce)
{
    php_gcurl_share *sh = ecalloc(1, sizeof(php_gcurl_share) + zend_object_properties_size(ce));

    zend_object_std_init(&sh->std, ce);
    object_properties_init(&sh->std, ce);
    sh->std.handlers = &gcurl_share_handlers;

    sh->share = curl_share_init();
    if (!sh->share) {
        zend_throw_error(NULL, "gcurl: Failed to initialize curl share handle");
        return &sh->std;
    }

    return &sh->std;
}

void gcurl_share_free_obj(zend_object *object)
{
    php_gcurl_share *sh = gcurl_share_from_obj(object);

    if (sh->share) {
        curl_share_cleanup(sh->share);
        sh->share = NULL;
    }

    zend_object_std_dtor(&sh->std);
}

PHP_FUNCTION(gcurl_share_init)
{
    ZEND_PARSE_PARAMETERS_NONE();

    object_init_ex(return_value, gcurl_share_handle_ce);
    php_gcurl_share *sh = Z_GCURL_SHARE_P(return_value);
    if (!sh->share) {
        RETURN_FALSE;
    }
}

PHP_FUNCTION(gcurl_share_setopt)
{
    zval      *z_sh, *z_val;
    zend_long  option;

    ZEND_PARSE_PARAMETERS_START(3, 3)
        Z_PARAM_OBJECT_OF_CLASS(z_sh, gcurl_share_handle_ce)
        Z_PARAM_LONG(option)
        Z_PARAM_ZVAL(z_val)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_share *sh = Z_GCURL_SHARE_P(z_sh);
    if (!sh->share) {
        RETURN_FALSE;
    }

    CURLSHcode ret = curl_share_setopt(sh->share, (CURLSHoption)option, zval_get_long(z_val));
    RETURN_BOOL(ret == CURLSHE_OK);
}

PHP_FUNCTION(gcurl_share_close)
{
    zval *z_sh;

    ZEND_PARSE_PARAMETERS_START(1, 1)
        Z_PARAM_OBJECT_OF_CLASS(z_sh, gcurl_share_handle_ce)
    ZEND_PARSE_PARAMETERS_END();

    php_gcurl_share *sh = Z_GCURL_SHARE_P(z_sh);
    if (sh->share) {
        curl_share_cleanup(sh->share);
        sh->share = NULL;
    }
}
