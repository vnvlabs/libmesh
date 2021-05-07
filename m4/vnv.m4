dnl -------------------------------------------------------------
dnl vnv 
dnl -------------------------------------------------------------
AC_DEFUN([CONFIGURE_VNV],
[
  AC_ARG_ENABLE(vnv,
                AS_HELP_STRING([--disable-vnv],
                               [build without VNV support]),
                [AS_CASE("${enableval}",
                         [yes], [enablevnv=yes],
                         [no],  [enablevnv=no],
                         [AC_MSG_ERROR(bad value ${enableval} for --enable-vnv)])],
                [enablevnv=$enableoptional])



  AC_ARG_WITH(vnv,
             AS_HELP_STRING([--with-vnv-include=</some/includedir>]),
             [VNV_INCLUDE="-I${withval}/include"
              VNV_LIB="-L${withval}/lib -linjection"
              enablevnv=yes
             ],
             [])



  dnl The VNV API is distributed with libmesh, so we don't have to guess
  dnl where it might be installed...
  AS_IF([test "$enablevnv" = "yes"],
        [
          dnl Only put Metis contrib directories into the compiler include paths if we're building our own Metis.
          AC_DEFINE(HAVE_VNV, 1, [Flag indicating whether the library will be compiled with Metis support])
          AC_MSG_RESULT(<<< Configuring library with Metis support >>>)

          dnl look for thread-local storage
          AX_TLS
        ],
        [
          VNV_INCLUDE=""
          VNV_LIB=""
          enablevnv=no
        ])

  AC_SUBST(VNV_INCLUDE)
  AC_SUBST(VNV_LIB)
])
