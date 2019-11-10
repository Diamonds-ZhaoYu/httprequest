dnl $Id$
dnl config.m4 for extension httprequest

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(httprequest, for httprequest support,
dnl Make sure that the comment is aligned:
dnl [  --with-httprequest             Include httprequest support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(httprequest, whether to enable httprequest support,
dnl Make sure that the comment is aligned:
[  --enable-httprequest           Enable httprequest support])

if test "$PHP_HTTPREQUEST" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-httprequest -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/httprequest.h"  # you most likely want to change this
  dnl if test -r $PHP_HTTPREQUEST/$SEARCH_FOR; then # path given as parameter
  dnl   HTTPREQUEST_DIR=$PHP_HTTPREQUEST
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for httprequest files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       HTTPREQUEST_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$HTTPREQUEST_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the httprequest distribution])
  dnl fi

  dnl # --with-httprequest -> add include path
  dnl PHP_ADD_INCLUDE($HTTPREQUEST_DIR/include)

  dnl # --with-httprequest -> check for lib and symbol presence
  dnl LIBNAME=httprequest # you may want to change this
  dnl LIBSYMBOL=httprequest # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $HTTPREQUEST_DIR/$PHP_LIBDIR, HTTPREQUEST_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_HTTPREQUESTLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong httprequest lib version or lib not found])
  dnl ],[
  dnl   -L$HTTPREQUEST_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(HTTPREQUEST_SHARED_LIBADD)

  PHP_NEW_EXTENSION(httprequest, httprequest.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
