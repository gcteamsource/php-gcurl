#ifndef GCURL_SHARE_H
#define GCURL_SHARE_H

#include "php_gcurl.h"

zend_object *gcurl_share_new(zend_class_entry *ce);
void         gcurl_share_free_obj(zend_object *object);

PHP_FUNCTION(gcurl_share_init);
PHP_FUNCTION(gcurl_share_setopt);
PHP_FUNCTION(gcurl_share_close);

#endif /* GCURL_SHARE_H */
