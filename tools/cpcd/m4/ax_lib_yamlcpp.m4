# ===========================================================================
#      https://www.gnu.org/software/autoconf-archive/ax_lib_yamlcpp.html
# ===========================================================================
#
# SYNOPSIS
#
#   AX_LIB_YAMLCPP()
#
# DESCRIPTION
#
#   This macro provides tests of the availability of the yaml-cpp library.
#
#   The macro adds a --with-yamlcpp option accepting one of three values:
#
#     no   - do not check for the yaml-cpp library.
#     yes  - do check for yaml-cpp library in standard locations.
#     path - installation prefix for yaml-cpp version.
#
#   If yaml-cpp is successfully found, this macro calls
#
#     AC_SUBST(YAMLCPP_CPPFLAGS)
#     AC_SUBST(YAMLCPP_LDFLAGS)
#     AC_SUBST(YAMLCPP_LIBS)
#     AC_DEFINE(HAVE_YAMLCPP)
#
#   It also sets
#
#     with_yamlcpp="yes"
#
#   If yaml-cpp is disabled or not found, this macros sets
#
#     with_yamlcpp="no"
#
#   Your configuration script can test $with_yamlcpp to take any further
#   actions. YAMLCPP_CPPFLAGS, YAMLCPP_LDFLAGS, and YAMLCPP_LIBS should be
#   used when building C++ applications.
#
#   To use the macro, one would add the following lines to "configure.ac"
#   before AC_OUTPUT:
#
#     dnl Check for yaml-cpp support
#     AX_LIB_YAMLCPP()
#
#   One could test $with_yamlcpp for the outcome or display it as follows
#
#     echo "yaml-cpp support: $with_yamlcpp"
#
#   Note that yaml-cpp requires the C++11 standard. Proper compiler flags
#   should be added to CXXFLAGS before invoking this macro. This can be
#   accomplished, for instance, by using macro:
#
#     AX_CXX_COMPILE_STDCXX([11],[noext],[mandatory])
#
# LICENSE
#
#   Copyright (c) 2019 National Earth System Prediction Capability/CSC
#
#   Copying and distribution of this file, with or without modification, are
#   permitted in any medium without royalty provided the copyright notice
#   and this notice are preserved. This file is offered as-is, without any
#   warranty.

#serial 1

AC_DEFUN([AX_LIB_YAMLCPP], [
dnl Check first argument is one of the recognized values.
dnl Fail eagerly if is incorrect as this simplifies case statements below.
if   test "m4_normalize(m4_default([$1],[]))" = ""        ; then
  ax_lib_yamlcpp_enable="yes"
elif test "m4_normalize(m4_default([$1],[]))" = "yes"  ; then
  ax_lib_yamlcpp_enable="yes"
elif test "m4_normalize(m4_default([$1],[]))" = "no"; then
  ax_lib_yamlcpp_enable="no"
else
  AC_MSG_ERROR([
    Unrecognized value for AX[]_LIB_YAMLCPP within configure.ac.
    If supplied, argument 1 must be either 'yes' or 'no'.
  ])
fi

dnl Add a default --with-yaml-cpp configuration option.
AC_ARG_WITH([yamlcpp],
  AS_HELP_STRING(
    [--with-yamlcpp=[yes/no/PATH]],
      [use yaml-cpp library (default: yes)]
  ),
  [if test "$withval" = "no"; then
     with_yamlcpp="no"
   elif test "$withval" = "yes"; then
     with_yamlcpp="yes"
   else
     with_yamlcpp="yes"
     ax_lib_yamlcpp_prefix="${withval}"
   fi],
  [with_yamlcpp=$ax_lib_yamlcpp_enable]
)

dnl Set defaults to blank
YAMLCPP_CPPFLAGS=""
YAMLCPP_LDFLAGS=""
YAMLCPP_LIBS=""

dnl Try to find YAMLCPP installation paths
ax_lib_yamlcpp_includdedir=""
ax_lib_yamlcpp_libdir=""
ax_lib_yamlcpp_library=""

if test "$with_yamlcpp" = "yes"; then
  AC_MSG_CHECKING([for yaml-cpp library])
  if test -z "$ax_lib_yamlcpp_prefix"; then
    dnl Retrieve settings from environment
    if test -z "$YAML_CPP"; then
      with_yamlcpp="no"
      AC_MSG_RESULT([no])
      AC_MSG_WARN([Environment variable YAML_CPP undefined])
    else
      ax_lib_yamlcpp_prefix=$YAML_CPP
    fi
  fi

  if ! test -z "$ax_lib_yamlcpp_prefix"; then
    dnl Check prefix directory
    if test -d "$ax_lib_yamlcpp_prefix" ; then

      dnl Assume standard installation if prefix is provided
      ax_lib_yamlcpp_includedir=${ax_lib_yamlcpp_prefix}/include
      ax_lib_yamlcpp_libdir=${ax_lib_yamlcpp_prefix}/lib
      ax_lib_yamlcpp_library=${ax_lib_yamlcpp_libdir}/libyaml-cpp.a
  
      dnl Verify installation paths
      dnl includedir
      if ! test -d "$ax_lib_yamlcpp_includedir" ; then
        with_yamlcpp="no"
        AC_MSG_RESULT([no])
        AC_MSG_WARN([Unable to find yaml-cpp include dir: ${ax_lib_yamlcpp_includedir}])
      fi

      dnl libdir
      if test "$with_yamlcpp" = "yes" ; then
        if ! test -d "$ax_lib_yamlcpp_libdir" ; then
          with_yamlcpp="no"
          AC_MSG_RESULT([no])
          AC_MSG_WARN(["Unable to find yaml-cpp lib dir: ${ax_lib_yamlcpp_libdir}"])
        fi
      fi

      dnl library file
      if test "$with_yamlcpp" = "yes" ; then
        if ! test -f "$ax_lib_yamlcpp_library" ; then
          with_yamlcpp="no"
          AC_MSG_RESULT([no])
          AC_MSG_WARN(["Unable to find yaml-cpp library: ${ax_lib_yamlcpp_library}"])
        else
          AC_MSG_RESULT([yes])
        fi
      fi

    else

      with_yamlcpp="no"
      AC_MSG_RESULT([no])
      AC_MSG_WARN(["Unable to find yaml-cpp installation path: ${ax_lib_yamlcpp_prefix}"])

    fi
  fi
fi

if test "$with_yamlcpp" = "yes"; then

  dnl Check if the library works
  AC_MSG_CHECKING([if yaml-cpp library works])
  AC_LANG_PUSH([C++])
  ax_lib_yamlcpp_save_CPPFLAGS="${CPPFLAGS}"
  ax_lib_yamlcpp_save_LDFLAGS="${LDFLAGS}"
  ax_lib_yamlcpp_save_LIBS="${LIBS}"
  CPPFLAGS="${CPPFLAGS} -I $ax_lib_yamlcpp_includedir"
  LDFLAGS="${LDFLAGS} -L$ax_lib_yamlcpp_libdir"
  LIBS="${LIBS} -lyaml-cpp"
  AC_LINK_IFELSE([AC_LANG_PROGRAM([dnl
       #include "yaml-cpp/yaml.h"],[dnl
       YAML::Node node
    ])],
    [AC_MSG_RESULT(yes)],
    [dnl library cannot compile
     with_yamlcpp="no"
     AC_MSG_RESULT(no)
  ])
  CPPFLAGS=$ax_lib_yamlcpp_save_CPPFLAGS
  LDFLAGS=$ax_lib_yamlcpp_save_LDFLAGS
  LIBS=$ax_lib_yamlcpp_save_LIBS
  AC_LANG_POP()

fi

if test "$with_yamlcpp" = "yes"; then

  YAMLCPP_CPPFLAGS=-I$ax_lib_yamlcpp_includedir
  YAMLCPP_LDFLAGS=-L$ax_lib_yamlcpp_libdir
  YAMLCPP_LIBS=-lyaml-cpp

  AC_SUBST([YAMLCPP_CPPFLAGS])
  AC_SUBST([YAMLCPP_LDFLAGS])
  AC_SUBST([YAMLCPP_LIBS])
  AC_DEFINE([HAVE_YAMLCPP], [1], [Defined if you have yaml-cpp support])

fi
])
