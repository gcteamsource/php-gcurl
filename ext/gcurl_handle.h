#ifndef GCURL_HANDLE_H
#define GCURL_HANDLE_H

#include "php_gcurl.h"

PHP_FUNCTION(gcurl_init);
PHP_FUNCTION(gcurl_impersonate);
PHP_FUNCTION(gcurl_setopt);
PHP_FUNCTION(gcurl_setopt_array);
PHP_FUNCTION(gcurl_exec);
PHP_FUNCTION(gcurl_getinfo);
PHP_FUNCTION(gcurl_errno);
PHP_FUNCTION(gcurl_error);
PHP_FUNCTION(gcurl_strerror);
PHP_FUNCTION(gcurl_reset);
PHP_FUNCTION(gcurl_close);
PHP_FUNCTION(gcurl_copy_handle);

#endif /* GCURL_HANDLE_H */
