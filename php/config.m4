dnl $Id$
dnl config.m4 for extension plame

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

dnl PHP_ARG_WITH(plame, for plame support,
dnl Make sure that the comment is aligned:
dnl [  --with-plame             Include plame support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(plame, whether to enable plame support,
dnl Make sure that the comment is aligned:
[  --enable-plame           Enable plame support])

if test "$PHP_PLAME" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-plame -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/plame.h"  # you most likely want to change this
  dnl if test -r $PHP_PLAME/$SEARCH_FOR; then # path given as parameter
  dnl   PLAME_DIR=$PHP_PLAME
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for plame files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       PLAME_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$PLAME_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the plame distribution])
  dnl fi

  dnl # --with-plame -> add include path
  dnl PHP_ADD_INCLUDE($PLAME_DIR/include)

  dnl # --with-plame -> check for lib and symbol presence
  dnl LIBNAME=plame # you may want to change this
  dnl LIBSYMBOL=plame # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $PLAME_DIR/$PHP_LIBDIR, PLAME_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_PLAMELIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong plame lib version or lib not found])
  dnl ],[
  dnl   -L$PLAME_DIR/$PHP_LIBDIR -lm
  dnl ])
  dnl
  dnl PHP_SUBST(PLAME_SHARED_LIBADD)

  PHP_REQUIRE_CXX()
  PHP_ADD_LIBRARY(stdc++, 1, PLAME_SHARED_LIBADD)
  PHP_ADD_LIBRARY_WITH_PATH(lame, /usr/local/lib, PLAME_SHARED_LIBADD)
  PHP_SUBST(PLAME_SHARED_LIBADD)
  PHP_NEW_EXTENSION(plame, plame.c, $ext_shared)
fi
