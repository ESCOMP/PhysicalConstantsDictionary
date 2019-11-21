#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.63])
AC_INIT([cpcd], [0.0.1], [raffaele.montuoro@noaa.gov])
AC_CONFIG_SRCDIR([include/syntax.h])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX

# C++11 standard is required.
AX_CXX_COMPILE_STDCXX([11],[noext],[mandatory])

# Checks for libraries.
AX_LIB_YAMLCPP([yes])
AS_IF([test "x$with_yamlcpp" = xno],
  [AC_MSG_ERROR([Unable to find required yaml-cpp library])],
  [CPPFLAGS="$CPPFLAGS $YAMLCPP_CPPFLAGS"
   LDFLAGS="$LDFLAGS $YAMLCPP_LDFLAGS"
   LIBS="$LIBS $YAMLCPP_LIBS"]
)

# Generate Makefiles
AC_CONFIG_FILES([Makefile
                 src/Makefile])

AC_OUTPUT
