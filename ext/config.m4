dnl config.m4 for PHP extension gcurl

PHP_ARG_WITH([gcurl],
  [for gcurl support],
  [AS_HELP_STRING([--with-gcurl=DIR], [Path to libcurl-impersonate installation prefix])],
  [no]
)

if test "$PHP_GCURL" != "no"; then
  AC_MSG_CHECKING([for libcurl-impersonate in default or specified path])
  GCURL_DIR=""
  for i in "$PHP_GCURL" /usr/local /opt/gcurl /usr; do
    if test -f "$i/include/curl/curl.h"; then
      GCURL_DIR="$i"
      break
    fi
  done

  if test -z "$GCURL_DIR"; then
    AC_MSG_ERROR([curl/curl.h not found. Specify --with-gcurl=<DIR>])
  fi
  AC_MSG_RESULT([found in $GCURL_DIR])

  PHP_ADD_INCLUDE([$GCURL_DIR/include])

  dnl Check for libcurl-impersonate library and curl_easy_impersonate function
  PHP_CHECK_LIBRARY([curl-impersonate], [curl_easy_impersonate],
  [
    PHP_ADD_LIBRARY_WITH_PATH([curl-impersonate], [$GCURL_DIR/lib], [GCURL_SHARED_LIBADD])
    AC_DEFINE([HAVE_GCURL], [1], [Have gcurl support])
  ], [
    AC_MSG_ERROR([libcurl-impersonate not found or curl_easy_impersonate missing in $GCURL_DIR/lib])
  ], [
    -L$GCURL_DIR/lib
  ])

  PHP_SUBST([GCURL_SHARED_LIBADD])
  PHP_NEW_EXTENSION([gcurl], [gcurl.c gcurl_handle.c gcurl_multi.c gcurl_share.c gcurl_constants.c], [$ext_shared],, [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
fi
