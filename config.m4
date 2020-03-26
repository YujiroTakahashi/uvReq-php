dnl config.m4 for extension uvRequest

PHP_ARG_WITH(uvrequest, for uvRequest support, [  --enable-uvrequest         Enable fasttext uvRequest])

if test "$PHP_UVREQUEST" != "no"; then
    SEARCH_PATH="/usr/local/include /usr/include /usr/include/x86_64-linux-gnu"
    SEARCH_FOR="/curl/curl.h"
    if test -r $PHP_UVREQUEST/$SEARCH_FOR; then # path given as parameter
        CURL_DIR=$PHP_UVREQUEST
    else # search default path list
        AC_MSG_CHECKING([for cURL files in default path])
        for i in $SEARCH_PATH ; do
            if test -r $i/$SEARCH_FOR; then
                CURL_DIR=$i
                AC_MSG_RESULT(found in $i)
            fi
        done
    fi
    if test -z "$CURL_DIR"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Please reinstall the cURL distribution, "$PHP_UVREQUEST"])
    fi


    SEARCH_PATH="/usr/local /usr"
    SEARCH_FOR="/include/uv.h"
    if test -r $PHP_UVREQUEST/$SEARCH_FOR; then # path given as parameter
        UL_DIR=$PHP_UVREQUEST
    else # search default path list
        AC_MSG_CHECKING([for libuv files in default path])
        for i in $SEARCH_PATH ; do
            if test -r $i/$SEARCH_FOR; then
                UV_DIR=$i
                AC_MSG_RESULT(found in $i)
            fi
        done
    fi
    if test -z "$UV_DIR"; then
        AC_MSG_RESULT([not found])
        AC_MSG_ERROR([Please reinstall the libuv distribution, "$PHP_UVREQUEST"])
    fi

    PHP_REQUIRE_CXX()

    # --with-uvrequest -> add include path
    PHP_ADD_INCLUDE($THIS_INCLUDE)
    PHP_SUBST(UVREQUEST_SHARED_LIBADD)

    PHP_ADD_LIBRARY(stdc++, 1, UVREQUEST_SHARED_LIBADD)
    PHP_ADD_LIBRARY(curl, 1, UVREQUEST_SHARED_LIBADD)
    PHP_ADD_LIBRARY(uv, 1, UVREQUEST_SHARED_LIBADD)
    CFLAGS="-O3 -funroll-loops"
    CXXFLAGS="-pthread -std=c++14 -funroll-loops -O3 -march=native"

    PHP_NEW_EXTENSION(uvrequest, classes/uvreq.cc src/uvrequest.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
