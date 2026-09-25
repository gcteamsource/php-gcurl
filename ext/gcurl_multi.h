#ifndef GCURL_MULTI_H
#define GCURL_MULTI_H

#include "php_gcurl.h"

zend_object *gcurl_multi_new(zend_class_entry *ce);
void         gcurl_multi_free_obj(zend_object *object);

PHP_FUNCTION(gcurl_multi_init);
PHP_FUNCTION(gcurl_multi_add_handle);
PHP_FUNCTION(gcurl_multi_remove_handle);
PHP_FUNCTION(gcurl_multi_exec);
PHP_FUNCTION(gcurl_multi_select);
PHP_FUNCTION(gcurl_multi_info_read);
PHP_FUNCTION(gcurl_multi_getcontent);
PHP_FUNCTION(gcurl_multi_errno);
PHP_FUNCTION(gcurl_multi_strerror);
PHP_FUNCTION(gcurl_multi_close);
PHP_FUNCTION(gcurl_multi_setopt);

#endif /* GCURL_MULTI_H */
