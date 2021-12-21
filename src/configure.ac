
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([dmserver], [15.1.1])
AM_INIT_AUTOMAKE([subdir-objects])

AC_CONFIG_SRCDIR([iDMS/dbC_functions.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

PKG_CHECK_MODULES([CHECK], [check >= 0.9.6])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_CXX
AC_PROG_LEX
AC_PROG_YACC
AC_PROG_SED
AC_PROG_LN_S
AC_PROG_MKDIR_P
AC_PROG_INSTALL

# Fix SLES11 retard
#AM_PROG_AR

LT_INIT([disable-static])
LT_LANG([C++])


# Checks for libraries.
# FIXME: Replace `main' with a function in `-lcl':
# AC_CHECK_LIB([cl], [main])

# Checks for header files.
# AC_CHECK_HEADERS([arpa/inet.h fcntl.h limits.h malloc.h memory.h netdb.h netinet/in.h stddef.h stdlib.h string.h strings.h sys/ioctl.h sys/param.h sys/socket.h sys/time.h sys/timeb.h termios.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_MODE_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
#AC_FUNC_FORK
#AC_FUNC_MALLOC
#AC_FUNC_MKTIME
#AC_FUNC_REALLOC
#AC_CHECK_FUNCS([alarm dup2 getcwd gethostbyname gethostname gettimeofday memset mkdir putenv re_comp realpath regcmp regcomp select socket strcasecmp strchr strdup strerror strncasecmp strpbrk strrchr strstr strtol strtoul])

AC_ARG_WITH([synchronicity_header],[AS_HELP_STRING([--with-synchronicity_header],[Directory containing Synchronicity scapi.h])],[SYNCHRONICITY_CPPFLAGS=$withval])
AC_ARG_WITH([synchronicity_ldflags],[AS_HELP_STRING([--with-synchronicity_ldflags],[LDFLAGS when linking Synchronicity])],[SYNCHRONICITY_LDFLAGS=$withval])
AC_ARG_WITH([synchronicity_libs],[AS_HELP_STRING([--with-synchronicity_libs],[LIBS when linking Synchronicity])],[SYNCHRONICITY_LIBS=$withval])
AC_ARG_WITH([version_id],[AS_HELP_STRING([--with-version_id],[Version ID used to identify dmserver, defaults to opus44ww_default])],[VERSION_ID=$withval],[VERSION_ID="opus44ww_default"])


AC_ARG_ENABLE([pic_mode],[AS_HELP_STRING([--enable-picmode],[Use -fPIC/-fPIE where available])],[
  FPIC_CPPFLAGS="-fPIC -fPIE"
  FPIE_LDFLAGS="-pie"
  ])

AC_ARG_ENABLE([stack_protect],[AS_HELP_STRING([--enable-stack_protect],[Use -fPIC/-fPIE where available])],[
  STACK_CPPFLAGS="-fstack-protector-all"
  STACK_LDFLAGS="-fstack-protector-all"
  ])

#AC_ARG_ENABLE(gcov,
#AC_HELP_STRING([--enable-gcov],
#               [turn on test coverage @<:@default=no@:>@]),
#[case "${enableval}" in
#  yes) enable_gcov=true ;;
#  no)  enable_gcov=true ;;
#  *)   AC_MSG_ERROR(bad value ${enableval} for --enable-gcov) ;;
#esac], [enable_gcov=false ])

#if test x$enable_gcov = xtrue ; then
#  if test x"$GCC" != xyes; then
#    AC_MSG_ERROR([gcov only works if gcc is used])
#  fi

 # GCOV_CFLAGS="-fprofile-arcs -ftest-coverage -lgcov"
 # AC_SUBST(GCOV_CFLAGS)

  dnl libtool 1.5.22 and lower strip -fprofile-arcs from the flags
  dnl passed to the linker, which is a bug; -fprofile-arcs implicitly
  dnl links in -lgcov, so we do it explicitly here for the same effect
  #GCOV_LIBS="-lgcov --coverage"
  #AC_SUBST(GCOV_LIBS)
#fi

#AM_CONDITIONAL(ENABLE_GCOV, test x"$enable_gcov" = "xtrue")

AC_CHECK_PROGS(GCOV, gcov, false)
AC_CHECK_PROGS(LCOV, lcov, false)
AC_CHECK_PROGS(GENHTML, genhtml, false)
AC_CHECK_PROGS(TEX, tex, false)
if test "$TEX" = "false"; then
    # Make it [somewhat] clear to maintainers that tex is missing. Not an error
    # though because 'make install' (which users need) does not build the docs
    # anyway.
    AC_MSG_WARN(tex not installed: cannot rebuild HTML documentation.)
fi

#/p/dt/cad/x86-64_linux26/synchronicity/V6B2012x/syncinc/scapi
CPPFLAGS_OLD="$CPPFLAGS"
CPPFLAGS="$CPPFLAGS -I$SYNCHRONICITY_CPPFLAGS"
AC_CHECK_HEADER([scapi.h],[],[AC_MSG_ERROR([Synchronicity headers not found])])
CPPFLAGS="$CPPFLAGS_OLD"

LDFLAGS_OLD="$LDFLAGS"
LIBS_OLD="$LIBS"
LDFLAGS="$LDFLAGS $SYNCHRONICITY_LDFLAGS"
LIBS="$LIBS $SYNCHRONICITY_LIBS"
#/p/dt/cad/x86-64_linux26/synchronicity/V6B2012x/syncinc/lib.Linux
AC_LANG([C])
AC_CHECK_LIB([scapi],[main],[],[AC_MSG_ERROR([Synchronicity lib not found])])
LIBS="$LIBS_OLD"
LDFLAGS="$LDFLAGS_OLD"

AC_SUBST([SYNCHRONICITY_CPPFLAGS])
AC_SUBST([SYNCHRONICITY_LDFLAGS])
AC_SUBST([SYNCHRONICITY_LIBS])
AC_SUBST([MARS_PATH])
AC_SUBST([VERSION_ID])
AC_SUBST([FPIC_CPPFLAGS])
AC_SUBST([FPIE_LDFLAGS])
AC_SUBST([STACK_CPPFLAGS])
AC_SUBST([STACK_LDFLAGS])
AC_SUBST([LIBTOOL_DEPS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

