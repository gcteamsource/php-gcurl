#ifndef PHP_GCURL_H
#define PHP_GCURL_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_interfaces.h"
#include "zend_exceptions.h"
#include "zend_smart_str.h"

#include <curl/curl.h>
#include <curl/easy.h>
#include <curl/multi.h>

#define PHP_GCURL_VERSION "1.0.0"
#define PHP_GCURL_EXTNAME "gcurl"

/* Custom CURLOPT values mirroring PHP ext/curl */
#define GCURLOPT_RETURNTRANSFER 19913
#define GCURLOPT_BINARYTRANSFER 19914

#define PHP_GCURL_STDOUT  0
#define PHP_GCURL_FILE    1
#define PHP_GCURL_USER    2
#define PHP_GCURL_DIRECT  3
#define PHP_GCURL_RETURN  4
#define PHP_GCURL_IGNORE  7

#define SAVE_GCURL_ERROR(_h, _code) do { \
    (_h)->err.no = (int)(_code); \
} while (0)

typedef struct _php_gcurl_handlers {
    int  write_method;
    int  read_method;
    int  header_method;
    zval write;
    zval read;
    zval header;
    zval progress;
    zval std_err;
} php_gcurl_handlers;

typedef struct _php_gcurl_error {
    char str[CURL_ERROR_SIZE + 1];
    int  no;
} php_gcurl_error;

typedef struct _php_gcurl_handle {
    CURL               *cp;
    php_gcurl_handlers  handlers;
    php_gcurl_error     err;
    bool                in_callback;
    struct curl_slist  *header;
    struct curl_slist  *postquote;
    struct curl_slist  *resolve;
    struct curl_slist  *connect_to;
    zval                postfields;
    smart_str           write_str;
    smart_str           header_str;
    zend_object         std; /* MUST BE LAST */
} php_gcurl_handle;

typedef struct _php_gcurl_multi {
    CURLM      *multi;
    HashTable   easyh;
    zend_object std; /* MUST BE LAST */
} php_gcurl_multi;

typedef struct _php_gcurl_share {
    CURLSH     *share;
    zend_object std; /* MUST BE LAST */
} php_gcurl_share;

static inline php_gcurl_handle *gcurl_handle_from_obj(zend_object *obj) {
    return (php_gcurl_handle *)((char *)(obj) - XtOffsetOf(php_gcurl_handle, std));
}

static inline php_gcurl_multi *gcurl_multi_from_obj(zend_object *obj) {
    return (php_gcurl_multi *)((char *)(obj) - XtOffsetOf(php_gcurl_multi, std));
}

static inline php_gcurl_share *gcurl_share_from_obj(zend_object *obj) {
    return (php_gcurl_share *)((char *)(obj) - XtOffsetOf(php_gcurl_share, std));
}

#define Z_GCURL_HANDLE_P(zv)  gcurl_handle_from_obj(Z_OBJ_P(zv))
#define Z_GCURL_MULTI_P(zv)   gcurl_multi_from_obj(Z_OBJ_P(zv))
#define Z_GCURL_SHARE_P(zv)   gcurl_share_from_obj(Z_OBJ_P(zv))

extern zend_class_entry *gcurl_handle_ce;
extern zend_class_entry *gcurl_multi_handle_ce;
extern zend_class_entry *gcurl_share_handle_ce;

extern zend_object_handlers gcurl_handle_handlers;
extern zend_object_handlers gcurl_multi_handlers;
extern zend_object_handlers gcurl_share_handlers;

zend_object *gcurl_handle_new(zend_class_entry *ce);
void gcurl_handle_free_obj(zend_object *object);

zend_object *gcurl_multi_new(zend_class_entry *ce);
void gcurl_multi_free_obj(zend_object *object);

zend_object *gcurl_share_new(zend_class_entry *ce);
void gcurl_share_free_obj(zend_object *object);

void gcurl_register_constants(int module_number);

#endif /* PHP_GCURL_H */
