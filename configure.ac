dnl Process this file with autoconf to produce a configure script.

PACKAGE=muzzley
VERSION=0.0.3

AC_PREREQ(2.59)
AC_INIT(muzzley, 0.0.3, pedro.figueiredo@muzzley.com)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])
	
AC_ARG_ENABLE(http, [  --enable-http		Turn on HTTP(S) client support (OpenSSL development library needed) ],
	[ AC_DEFINE(HAVE_HTTP_CLIENT, 1, "Turn on HTTP(S) client support - OpenSSL development library needed") ]
)

AC_ARG_ENABLE(ssl, [  --disable-ssl		Turn off SSL secure channel for real-time channel (OpenSSL development library needed) ],
	[ AC_DEFINE(NOT_HAVE_SECURE_CHANNEL, 1, "Turn off SSL secure channel for real-time channel - OpenSSL development library needed") ]
)

AC_ARG_ENABLE(logs, [  --enable-logs		Turn on log messages ],
	[ AC_DEFINE(MUZZLEY_DEBUG, 1, "Turn on log messages") ]
)

dnl Check for libraries
AC_HAVE_LIBRARY(pthread)
AC_HAVE_LIBRARY(rt)
if test -z $enable_ssl
then
	AC_HAVE_LIBRARY(ssl)
	AC_HAVE_LIBRARY(crypto)
fi

AC_CANONICAL_SYSTEM

dnl Create config.h header file
AC_CONFIG_HEADERS([include/muzzley/config.h])

dnl Initialize automake
AM_INIT_AUTOMAKE([subdir-objects])

dnl this allows us specify individual liking flags for each target
AC_PROG_CXX
AM_PROG_CC_C_O 

dnl Initialize Libtool
LT_INIT

dnl Check if Libtool is present
dnl Libtool is used for building share libraries 
AC_PROG_LIBTOOL

AC_CONFIG_FILES(Makefile
	include/Makefile
	src/Makefile)
AC_OUTPUT
