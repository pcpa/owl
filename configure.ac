dnl
dnl Copyright (C) 2013  Paulo Cesar Pereira de Andrade.
dnl
dnl This is free software; you can redistribute it and/or modify
dnl it under the terms of the GNU General Public License as published by
dnl the Free Software Foundation; either version 3 of the License, or
dnl (at your option) any later version.
dnl
dnl This software is distributed in the hope that it will be useful,
dnl but WITHOUT ANY WARRANTY; without even the implied warranty of
dnl MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
dnl GNU General Public License for more details.
dnl
dnl Authors:
dnl	Paulo Cesar Pereira de Andrade
dnl

dnl  Process this file with autoconf to create configure.

AC_PREREQ([2.57])
AC_INIT(owl, [0.0.1], , owl)
AC_CANONICAL_TARGET
AC_CONFIG_SRCDIR([Makefile.am])
AM_INIT_AUTOMAKE([dist-bzip2])
AC_CONFIG_MACRO_DIR([m4])

AM_CONFIG_HEADER(config.h)

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CHECK_LIB(gmp, __gmpz_init, ,
	[AC_MSG_ERROR([GNU MP not found, see http://gmplib.org/])])

AC_CHECK_LIB(mpfr, mpfr_init, ,
	[AC_MSG_ERROR([MPFR not found, see http://www.mpfr.org/])])

AC_CHECK_LIB(mpc, mpc_add, ,
	[AC_MSG_ERROR([MPC not found, see http://www.multiprecision.org/])])

AC_CHECK_HEADER([complex.h],
	[AC_DEFINE([HAVE_COMPLEX_H], 1,
		[Define to 1 if you have the <complex.h> header file.])],
	[AC_MSG_ERROR([C99 complex numbers support required])])

AC_CHECK_LIB(lightning, init_jit, ,
	[AC_MSG_ERROR([GNU lightning not found, see http://www.gnu.org/software/lightning/])])

AC_MSG_CHECKING([if need to use mpc_mul_2exp])
mpc_mul_2exp_ok=yes
save_LIBS=$LIBS
LIBS="$LIBS -lmpc -lmpfr -lgmp"
AC_TRY_LINK([#include <mpc.h>]
	    [mpc_t a, b;],
	    [mpc_mul_2exp(a, b, 1, 0);],
	AC_DEFINE([HAVE_MPC_MUL_2EXP], 1,
		[Define to 1 if need to use mpc_mul_2exp.]),
	[mpc_mul_2exp_ok=no])
AC_MSG_RESULT([$mpc_mul_2exp_ok])
LIBS=$save_LIBS

AC_MSG_CHECKING([if mpc_set_dc is available])
mpc_set_dc_ok=yes
save_LIBS=$LIBS
LIBS="$LIBS -lmpc -lmpfr -lgmp"
AC_TRY_LINK([#include <complex.h>]
	    [#include <mpc.h>]
	    [mpc_t a;]
	    [complex double b;],
	    [mpc_set_dc(a, b, 0);],
	AC_DEFINE([HAVE_MPC_SET_DC], 1,
		[Define to 1 if mpc_set_dc is available.]),
	[mpc_set_dc_ok=no])
AC_MSG_RESULT([$mpc_set_dc_ok])
LIBS=$save_LIBS

AC_MSG_CHECKING([for clog2 support])
clog2_ok=yes
save_LIBS=$LIBS
LIBS="$LIBS -lm"
AC_TRY_LINK([#include <complex.h>],
		[return = clog2(1.0);],
	AC_DEFINE([HAVE_CLOG2], 1,
		[Define to 1 if clog2 is available.]),
	[clog2_ok=no])
AC_MSG_RESULT([$clog2_ok])
LIBS=$save_LIBS

AC_MSG_CHECKING([for clog10 support])
clog10_ok=yes
save_LIBS=$LIBS
LIBS="$LIBS -lm"
AC_TRY_LINK([#include <complex.h>],
		[return = clog10(1.0);],
	AC_DEFINE([HAVE_CLOG10], 1,
		[Define to 1 if clog10 is available.]),
	[clog10_ok=no])
AC_MSG_RESULT([$clog10_ok])
LIBS=$save_LIBS

AC_MSG_CHECKING([for sincos support])
sincos_ok=yes
save_LIBS=$LIBS
LIBS="$LIBS -lm"
AC_TRY_LINK([double a, b;],
		[sincos(1.0, &a, &b);],
	AC_DEFINE([HAVE_SINCOS], 1,
		[Define to 1 if sincos is available.]),
	[sincos_ok=no])
AC_MSG_RESULT([$sincos_ok])
LIBS=$save_LIBS

AC_CHECK_HEADER([execinfo.h],
	[AC_DEFINE([HAVE_EXECINFO_H], 1,
		[Define to 1 if you have the <execinfo.h> header file.])],)

AC_CHECK_HEADER([pthread.h],
	[AC_DEFINE([HAVE_PTHREAD_H], 1,
		[Define to 1 if you have the <pthread.h> header file.])],
	[AC_MSG_ERROR([Pthread support required])])
OWL_CFLAGS="$OWL_CFLAGS -pthread"

AC_MSG_CHECKING([for TLS support])
have_tls=yes
save_CFLAGS=$CFLAGS
CFLAGS="-pthread"
AC_TRY_LINK([__thread void *tls_var;],
		[tls_var = malloc(1024);],
	AC_DEFINE([HAVE_TLS], 1,
		[Define to 1 if __thread is available.]),
	[have_tls=no])
AC_MSG_RESULT([$have_tls])
CFLAGS=$save_CFLAGS

dnl Debug/production build enable/disable
AC_ARG_ENABLE(debug,
	      AS_HELP_STRING([--enable-debug],
			     [Enable debug code, consistency checking, and assertions]),
	      [DEBUG=$withval], [DEBUG=no])
if test "x$DEBUG" != "xno"; then
    DEBUG_CFLAGS="-DDEBUG=1"
else
    DEBUG_CFLAGS="-DNDEBUG=1"
fi
AC_SUBST([DEBUG_CFLAGS])

WARN_CFLAGS="\
-Wall \
-Wbad-function-cast \
-Wdeclaration-after-statement \
-Wmissing-prototypes \
-Wmissing-declarations \
-Wnested-externs \
-fno-strict-aliasing \
-Wold-style-definition \
-Wpointer-arith \
-Wstrict-prototypes"
AC_SUBST([WARN_CFLAGS])
VISIBILITY_FLAGS="-fvisibility=hidden"

OWL_INC='-I$(top_srcdir)/include'
OWL_CFLAGS="$OWL_CFLAGS -std=c99 -fwrapv $VISIBILITY_FLAGS -D_GNU_SOURCE=1 $OWL_INC"
AC_SUBST([OWL_CFLAGS])
OWL_LIBS='$(top_builddir)/lib/libowl.la'
AC_SUBST([OWL_LIBS])

AC_OUTPUT([Makefile
	   check/Makefile
	   include/Makefile
	   lib/Makefile
	  ])
