AC_PREREQ([2.63])
AC_INIT([wpn], [0.4], [andrei.i.ivanov@commandus.com])
AM_INIT_AUTOMAKE([1.11 foreign no-define subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_SRCDIR([wpn.cpp])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG([C])
AC_LANG([C++])

m4_include([m4/ax_cxx_compile_stdcxx.m4])
# AX_CXX_COMPILE_STDCXX(11, noext, mandatory)

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_INSTALL
AC_ENABLE_SHARED
AC_PROG_LIBTOOL

AM_CONDITIONAL([DEBUG], [test x$debug = xtrue])

#PKG_CHECK_MODULES(PROTOBUF, protobuf >= 2.6.1)
#AC_SUBST(PROTOBUF_LIBS)
#AC_SUBST(PROTOBUF_CFLAGS)
#AC_SUBST(PROTOBUF_VERSION)

# MacOS: 
#   brew install openssl@1.1
# install openssl into /usr/local/lib/opt/openssl@1.1
LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/local/opt/openssl@1.1/lib -L/usr/local/lib64"

# Checks for header files. Add /usr/local/include for IS X.
CFLAGS="$CFLAGS -I/usr/local/include"
AC_CHECK_HEADERS([openssl/ssl.h], [], AC_MSG_ERROR([Could not find openssl/ssl.h. Try $ ./configure CFLAGS='-Iyour-openssl-include-path]))
AC_CHECK_HEADERS([curl/curl.h], [], AC_MSG_ERROR([Could not find curl/curl.h. Try $ ./configure CFLAGS='-Iyour-curl-include-path]))
AC_CHECK_HEADERS([fcntl.h limits.h stdint.h string.h sys/time.h termios.h unistd.h])

# Checks for libraries. Add /usr/local/lib for OS X
AC_CHECK_LIB([crypto], [EVP_CIPHER_CTX_reset], [], AC_MSG_ERROR([Could not find openssl >= 1.1.0. Try $ ./configure LDFLAGS='-Lyour-openssl2-lib-path']))
# AC_CHECK_LIB(curl, curl_easy_init, [], AC_MSG_ERROR([Could not find libcurl. Try $ ./configure LDFLAGS='-Lyour-curl-lib-path']))

# Checks for typedefs, structures, and compiler characteristics.
# AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT64_T

# Checks for library functions.
AC_FUNC_FORK
AC_CHECK_FUNCS([memmove memset])

AC_OUTPUT
