dnl -------------------------------------------------------------
dnl exodus
dnl -------------------------------------------------------------
AC_DEFUN([CONFIGURE_EXODUS],
[
  AC_ARG_ENABLE(exodus,
                AS_HELP_STRING([--disable-exodus],
                               [build without ExodusII API support]),
                [AS_CASE("${enableval}",
                         [yes],      [enableexodus=yes
                                      AS_IF([test "x$enablehdf5" = "xyes"],
                                            [exodusversion=v8.11],
                                            [exodusversion=v5.22])],
                         [v811],     [enableexodus=yes
                                      exodusversion="v8.11"],
                         [new|v522], [enableexodus=yes
                                      exodusversion="v5.22"],
                         [old|v509], [enableexodus=yes
                                      exodusversion="v5.09"],
                         [no],       [enableexodus=no
                                      exodusversion=no],
                         [AC_MSG_ERROR(bad value ${enableval} for --enable-exodus)])],
                [enableexodus=$enablenetcdf ;
                 AS_IF([test "x$enablehdf5" = "xyes"],
                       [exodusversion=v8.11],
                       [exodusversion=v5.22])]) dnl if unspecified, depend on netcdf+hdf5

  dnl fix for --disable-optional
  AS_IF([test "x$enableexodus" = "xno"], [exodusversion=no])

  dnl no HDF5 &/or no NETCDF4
  EXODUS_NOT_NETCDF4_FLAG=""
  AS_IF([test "x$enablehdf5" = "xno" || test "x$netcdfversion" = "x3"],
        [
          EXODUS_NOT_NETCDF4_FLAG="-DNOT_NETCDF4"
          AC_MSG_RESULT([defining -DNOT_NETCDF4 for our Exodus build])
        ])

  AS_CASE("${exodusversion}",
          ["v5.09"], [dnl The EXODUS API is distributed with libmesh, so we don't have to guess
                      dnl where it might be installed...
                      EXODUS_INCLUDE="-I\$(top_srcdir)/contrib/exodusii/$exodusversion/include"
                      AC_DEFINE(HAVE_EXODUS_API, 1, [Flag indicating whether the library will be compiled with Exodus support])
                      AC_DEFINE_UNQUOTED(DETECTED_EXODUS_VERSION_MAJOR, [5],
                                        [Exodus's major version number, as detected by exodus.m4])
                      AC_DEFINE_UNQUOTED(DETECTED_EXODUS_VERSION_MINOR, [9],
                                        [Exodus's minor version number, as detected by exodus.m4])
                      AC_MSG_RESULT(<<< Configuring library with Exodus version $exodusversion support >>>)
                      dnl Exodus Fortran API requires v5.22
                      enableexodusfortran=no],
          ["v5.22"], [EXODUS_INCLUDE="-I\$(top_srcdir)/contrib/exodusii/$exodusversion/exodus/cbind/include"
                      AC_DEFINE(HAVE_EXODUS_API, 1, [Flag indicating whether the library will be compiled with Exodus support])
                      AC_DEFINE_UNQUOTED(DETECTED_EXODUS_VERSION_MAJOR, [5],
                                        [Exodus's major version number, as detected by exodus.m4])
                      AC_DEFINE_UNQUOTED(DETECTED_EXODUS_VERSION_MINOR, [22],
                                        [Exodus's minor version number, as detected by exodus.m4])
                      AC_MSG_RESULT(<<< Configuring library with Exodus version $exodusversion support >>>)
                      AC_ARG_ENABLE(exodus-fortran,
                                    AS_HELP_STRING([--enable-exodus-fortran],
                                                   [build with ExodusII Fortran API support]),
                                    [AS_CASE("${enableval}",
                                             [yes], [enableexodusfortran=yes],
                                             [no],  [enableexodusfortran=no],
                                             [AC_MSG_ERROR(bad value ${enableval} for --enable-exodus-fortran)])],
                                    [enableexodusfortran=$enablefortran])
                      AS_IF([test "x$enableexodusfortran" = "xyes"],
                            [AC_MSG_RESULT(<<< Configuring library with Exodus Fortran API >>>)])],
          ["v8.11"], [EXODUS_INCLUDE="-I\$(top_srcdir)/contrib/exodusii/$exodusversion/exodus/include -I\$(top_srcdir)/contrib/exodusii/$exodusversion/exodus/sierra"
                      AC_DEFINE(HAVE_EXODUS_API, 1, [Flag indicating whether the library will be compiled with Exodus support])
                      AC_DEFINE_UNQUOTED(DETECTED_EXODUS_VERSION_MAJOR, [8],
                                        [Exodus's major version number, as detected by exodus.m4])
                      AC_DEFINE_UNQUOTED(DETECTED_EXODUS_VERSION_MINOR, [11],
                                        [Exodus's minor version number, as detected by exodus.m4])
                      AC_MSG_RESULT(<<< Configuring library with Exodus version $exodusversion support >>>)
                      dnl Exodus Fortran API requires v5.22
                      enableexodusfortran=no],
        [
          EXODUS_INCLUDE=""
          enableexodus=no
          enableexodusfortran=no
        ])

  AC_CONFIG_FILES([contrib/exodusii/v5.09/Makefile])
  AC_CONFIG_FILES([contrib/exodusii/v5.22/exodus/Makefile])
  AC_CONFIG_FILES([contrib/exodusii/v8.11/exodus/Makefile])
  AC_SUBST(EXODUS_INCLUDE)
  AC_SUBST(EXODUS_NOT_NETCDF4_FLAG)
  AM_CONDITIONAL(EXODUS_FORTRAN_API, test x$enableexodusfortran = xyes)
])
