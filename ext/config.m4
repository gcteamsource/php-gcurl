dnl config.m4 for PHP extension gcurl

PHP_ARG_WITH([gcurl],
  [for gcurl support],
  [AS_HELP_STRING([--with-gcurl=DIR], [Path to libcurl-impersonate installation prefix (default: search standard paths)])],
  [yes],
  [yes]
)

if test "$PHP_GCURL" != "no"; then
  AC_MSG_CHECKING([for libcurl-impersonate in default or specified path])
  GCURL_DIR=""
  SEARCH_PATHS=""

  if test "$PHP_GCURL" != "yes" && test -n "$PHP_GCURL"; then
    SEARCH_PATHS="$PHP_GCURL"
  else
    SEARCH_PATHS="/usr/local /opt/homebrew /opt/gcurl /usr/local/libcurl-impersonate /usr"
  fi

  for i in $SEARCH_PATHS; do
    if test -f "$i/include/curl/curl.h"; then
      GCURL_DIR="$i"
      break
    fi
  done

  if test -z "$GCURL_DIR"; then
    AC_MSG_ERROR([curl/curl.h not found. Please install libcurl-impersonate (https://github.com/lexiforest/curl-impersonate) or specify --with-gcurl=<DIR>])
  fi
  AC_MSG_RESULT([found in $GCURL_DIR])

  PHP_ADD_INCLUDE([$GCURL_DIR/include])

  dnl Determine library directory (lib or lib64)
  GCURL_LIB_DIR="$GCURL_DIR/lib"
  if test -d "$GCURL_DIR/lib64" && test ! -f "$GCURL_DIR/lib/libcurl-impersonate.so"; then
    GCURL_LIB_DIR="$GCURL_DIR/lib64"
  fi

  dnl Check for libcurl-impersonate library and curl_easy_impersonate function
  PHP_CHECK_LIBRARY([curl-impersonate], [curl_easy_impersonate],
  [
    PHP_ADD_LIBRARY_WITH_PATH([curl-impersonate], [$GCURL_LIB_DIR], [GCURL_SHARED_LIBADD])
    AC_DEFINE([HAVE_GCURL], [1], [Have gcurl support])
  ], [
    AC_MSG_ERROR([libcurl-impersonate not found or curl_easy_impersonate missing in $GCURL_LIB_DIR])
  ], [
    -L$GCURL_LIB_DIR
  ])

  PHP_SUBST([GCURL_SHARED_LIBADD])
  PHP_NEW_EXTENSION([gcurl], [gcurl.c gcurl_handle.c gcurl_multi.c gcurl_share.c gcurl_constants.c], [$ext_shared],, [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
fi
