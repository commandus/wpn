AC_PREREQ([2.63])
AC_INIT([ece], [0.9], [andrei.i.ivanov@commandus.com])
AM_INIT_AUTOMAKE([1.11 foreign no-define subdir-objects])
AC_CONFIG_FILES([Makefile])
AC_CONFIG_SRCDIR([encrypt.c])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LIBTOOL

# MacOS: 
#   brew install openssl@1.1
# install openssl into /usr/local/lib/opt/openssl@1.1
LDFLAGS="$LDFLAGS -L/usr/local/lib -L/usr/local/opt/openssl@1.1/lib"

# Checks for libraries. Add /usr/local/lib for OS X
AC_CHECK_LIB([crypto], [EVP_CIPHER_CTX_reset], [], AC_MSG_ERROR([Could not find openssl >= 1.1.0. Try $ ./configure LDFLAGS='-Lyour-openssl2-lib-path']))

# Checks for header files. Add /usr/local/include for IS X.
CFLAGS="$CFLAGS -I/usr/local/include"
AC_CHECK_HEADERS([ece.h], [], AC_MSG_ERROR([Could not find ece.h. Try $ ./configure CFLAGS='-Iyour-ece-include-path]))

AC_OUTPUT
