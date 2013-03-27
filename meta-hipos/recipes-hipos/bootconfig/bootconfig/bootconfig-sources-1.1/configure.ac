AC_PREREQ([2.60])
AC_INIT([bootconfig], [0.2], [fromm@dresearch-fe.de])
AC_CONFIG_AUX_DIR([build-aux])
AM_INIT_AUTOMAKE([-Wall -Werror foreign parallel-tests])
AC_CONFIG_HEADERS([config.h])

#------------------------------------------------------
# Checks for programs.

AC_PROG_CC

#------------------------------------------------------
# Checks for libraries.
# (Order is important! Check libs required by other libs first.)

AC_SEARCH_LIBS( libmtd_open, mtd, [], AC_MSG_ERROR([libmtd is required for building bootconfig. Please use [LDFLAGS]=-Ldir to specify the right location.] ) )


#------------------------------------------------------
# Checks for header files.

AC_HEADER_STDC

#------------------------------------------------------
# Checks for typedefs, structures, and compiler characteristics.

#AC_C_CONST
#AC_C_VOLATILE
#AC_TYPE_SIZE_T

#------------------------------------------------------

# Checks for library functions.
#AC_CHECK_FUNCS([getopt_long memset basename])

#------------------------------------------------------
# Checks for cc params
AC_DEFUN([CC_CHECK_CFLAGS_SILENT], [
  AC_CACHE_VAL(AS_TR_SH([cc_cv_cflags_$1]),
    [ac_save_CFLAGS="$CFLAGS"
     CFLAGS="$CFLAGS $1"
     AC_COMPILE_IFELSE([AC_LANG_SOURCE([int a;])],
       [eval "AS_TR_SH([cc_cv_cflags_$1])='yes'"],
       [eval "AS_TR_SH([cc_cv_cflags_$1])='no'"])
     CFLAGS="$ac_save_CFLAGS"
    ])

  AS_IF([eval test x$]AS_TR_SH([cc_cv_cflags_$1])[ = xyes], [$2], [$3])
])

AC_DEFUN([CC_CHECK_CFLAGS], [
  AC_CACHE_CHECK([if $CC supports $1 flag],
    AS_TR_SH([cc_cv_cflags_$1]),
    CC_CHECK_CFLAGS_SILENT([$1]) dnl Don't execute actions here!
  )

  AS_IF([eval test x$]AS_TR_SH([cc_cv_cflags_$1])[ = xyes], [$2], [$3])
])

CC_CHECK_CFLAGS([-Wall], [CFLAGS="$CFLAGS -Wall"])
CC_CHECK_CFLAGS([-Wextra], [CFLAGS="$CFLAGS -Wextra"])

#------------------------------------------------------
# OK, let's go
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
