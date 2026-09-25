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

  dnl Determine library directory (lib, lib64, or root prefix)
  GCURL_LIB_DIR="$GCURL_DIR/lib"
  if test -f "$GCURL_DIR/libcurl-impersonate.so"; then
    GCURL_LIB_DIR="$GCURL_DIR"
  elif test -d "$GCURL_DIR/lib64" && test ! -f "$GCURL_DIR/lib/libcurl-impersonate.so"; then
    GCURL_LIB_DIR="$GCURL_DIR/lib64"
  fi

  dnl Check for libcurl-impersonate library and curl_easy_impersonate function
  dnl Note: We use AC_LINK_IFELSE directly instead of PHP_CHECK_LIBRARY because
  dnl PHP_CHECK_LIBRARY generates "unset ac_cv_lib_curl-impersonate_..." which causes
  dnl "bad variable name" error on POSIX / BusyBox sh due to hyphen in library name.
  AC_MSG_CHECKING([for curl_easy_impersonate in -lcurl-impersonate])

  save_old_CPPFLAGS=$CPPFLAGS
  save_old_LDFLAGS=$LDFLAGS
  save_old_LIBS=$LIBS

  CPPFLAGS="-I$GCURL_DIR/include $CPPFLAGS"
  LDFLAGS="-L$GCURL_LIB_DIR $LDFLAGS"
  LIBS="-lcurl-impersonate $LIBS"

  AC_LINK_IFELSE([
    AC_LANG_PROGRAM([[
      #include <curl/curl.h>
    ]], [[
      curl_easy_impersonate(NULL, "chrome", 1);
    ]])
  ], [
    AC_MSG_RESULT([yes])
    PHP_ADD_LIBRARY_WITH_PATH([curl-impersonate], [$GCURL_LIB_DIR], [GCURL_SHARED_LIBADD])
    AC_DEFINE([HAVE_GCURL], [1], [Have gcurl support])
  ], [
    AC_MSG_RESULT([no])
    AC_MSG_ERROR([libcurl-impersonate not found or curl_easy_impersonate missing in $GCURL_LIB_DIR])
  ])

  CPPFLAGS=$save_old_CPPFLAGS
  LDFLAGS=$save_old_LDFLAGS
  LIBS=$save_old_LIBS

  PHP_SUBST([GCURL_SHARED_LIBADD])
  PHP_NEW_EXTENSION([gcurl], [gcurl.c gcurl_handle.c gcurl_multi.c gcurl_share.c gcurl_constants.c], [$ext_shared],, [-DZEND_ENABLE_STATIC_TSRMLS_CACHE=1])
fi
