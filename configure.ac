dnl Process this file with autoconf to produce a configure script.

PACKAGE=muzzley
VERSION=0.0.2

AC_PREREQ(2.59)
AC_INIT(muzzley, 0.0.2, pedro.figueiredo@muzzley.com)

AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([.])

dnl Check for libraries
AC_HAVE_LIBRARY(pthread)

AC_CANONICAL_SYSTEM

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
