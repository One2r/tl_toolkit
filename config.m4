dnl $Id$
dnl config.m4 for extension tl_toolkit

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(tl_toolkit, for tl_toolkit support,
dnl Make sure that the comment is aligned:
dnl [  --with-tl_toolkit             Include tl_toolkit support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(tl_toolkit, whether to enable tl_toolkit support,
Make sure that the comment is aligned:
[  --enable-tl_toolkit           Enable tl_toolkit support])

if test "$PHP_TL_TOOLKIT" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-tl_toolkit -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/tl_toolkit.h"  # you most likely want to change this
  dnl if test -r $PHP_TL_TOOLKIT/$SEARCH_FOR; then # path given as parameter
  dnl   TL_TOOLKIT_DIR=$PHP_TL_TOOLKIT
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for tl_toolkit files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       TL_TOOLKIT_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$TL_TOOLKIT_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the tl_toolkit distribution])
  dnl fi

  dnl # --with-tl_toolkit -> add include path
  dnl PHP_ADD_INCLUDE($TL_TOOLKIT_DIR/include)

  dnl # --with-tl_toolkit -> check for lib and symbol presence
  dnl LIBNAME=tl_toolkit # you may want to change this
  dnl LIBSYMBOL=tl_toolkit # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $TL_TOOLKIT_DIR/$PHP_LIBDIR, TL_TOOLKIT_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_TL_TOOLKITLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong tl_toolkit lib version or lib not found])
  dnl ],[
  dnl   -L$TL_TOOLKIT_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(TL_TOOLKIT_SHARED_LIBADD)

  PHP_NEW_EXTENSION(tl_toolkit, tl_toolkit.c tl_string.c, $ext_shared, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
